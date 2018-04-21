#include "component/controller/CodeController.h"

#include <memory>

#include "data/access/StorageAccess.h"
#include "data/location/SourceLocation.h"
#include "data/location/SourceLocationCollection.h"
#include "data/location/SourceLocationFile.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileInfo.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/tracing.h"
#include "utility/utilityString.h"
#include "Application.h"

CodeController::CodeController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

CodeController::~CodeController()
{
}

const uint CodeController::s_lineRadius = 2;

void CodeController::handleMessage(MessageActivateAll* message)
{
	TRACE("code all");

	saveOrRestoreViewMode(message);

	Project* currentProject = Application::getInstance()->getCurrentProject().get();
	if (!currentProject || message->acceptedNodeTypes != NodeTypeSet::all())
	{
		return;
	}

	CodeSnippetParams statsSnippet;
	statsSnippet.title = currentProject->getProjectSettingsFilePath().withoutExtension().fileName();

	statsSnippet.startLineNumber = 1;
	statsSnippet.endLineNumber = 1;

	statsSnippet.reduced = true;

	statsSnippet.locationFile = std::make_shared<SourceLocationFile>(FilePath(), true, true);

	std::vector<std::string> description = getProjectDescription(statsSnippet.locationFile.get());

	std::stringstream ss;

	if (description.size())
	{
		ss << "\n";
		for (const std::string& line : description)
		{
			ss << line + "\n";
		}
	}

	StorageStats stats = m_storageAccess->getStorageStats();
	ErrorCountInfo errorCount = m_storageAccess->getErrorCount();

	ss << "\n";
	if (stats.timestamp.isValid())
	{
		ss << "\tlast indexed: " + stats.timestamp.toString() + "\n";
		ss << "\n";
	}

	ss << "\t" + std::to_string(stats.fileCount) + " files";
	ss << (stats.completedFileCount != stats.fileCount ? " (" + std::to_string(stats.completedFileCount) + " complete)" : "") + "\n";
	ss << "\t" + std::to_string(stats.fileLOCCount) + " lines of code\n";
	ss << "\n";
	ss << "\t" + std::to_string(stats.nodeCount) + " symbols\n";
	ss << "\t" + std::to_string(stats.edgeCount) + " relations\n";
	ss << "\n";
	ss << "\t" + std::to_string(errorCount.total) + " errors (" + std::to_string(errorCount.fatal) + " fatal)\n";
	ss << "\n";

	if (stats.completedFileCount != stats.fileCount)
	{
		ss << "\tWarning: Indexing is incomplete as long as it yields fatal errors.\n";
		ss << "\tTry resolving them and refresh the project.\n";
		ss << "\n";
	}

	statsSnippet.code = ss.str();

	CodeView::CodeParams params;
	params.clearSnippets = true;
	params.showContents = !message->isReplayed();
	showCodeSnippets({ statsSnippet }, params);
}

void CodeController::handleMessage(MessageActivateLocalSymbols* message)
{
	CodeView* view = getView();
	view->showActiveLocalSymbolIds(message->symbolIds);
}

void CodeController::handleMessage(MessageActivateTokens* message)
{
	TRACE("code activate");

	saveOrRestoreViewMode(message);

	CodeView* view = getView();

	CodeView::CodeParams params;
	params.activeTokenIds = message->tokenIds;
	if (!params.activeTokenIds.size())
	{
		view->clear();
		return;
	}

	params.clearSnippets = !message->keepContent();
	params.showContents = !message->isReplayed();

	Id declarationId = 0; // 0 means that no token is found.
	if (!message->isAggregation)
	{
		params.activeTokenIds = m_storageAccess->getActiveTokenIdsForId(params.activeTokenIds[0], &declarationId);
	}

	if (message->isEdge)
	{
		std::shared_ptr<SourceLocationCollection> collection =
			m_storageAccess->getSourceLocationsForTokenIds(params.activeTokenIds);
		view->showActiveSnippet(params.activeTokenIds, collection, message->isLast());
		return;
	}

	if (message->keepContent())
	{
		view->showActiveTokenIds(params.activeTokenIds);
		return;
	}

	CodeView::ScrollParams scrollParams(CodeView::ScrollParams::SCROLL_TO_DEFINITION);
	scrollParams.ignoreActiveReference = true;
	view->scrollTo(scrollParams);

	m_collection = m_storageAccess->getSourceLocationsForTokenIds(params.activeTokenIds);
	showCodeSnippets(getSnippetsForActiveSourceLocations(m_collection.get(), declarationId), params);


	size_t fileCount = m_collection->getSourceLocationFileCount();
	size_t referenceCount = m_collection->getSourceLocationCount();

	std::wstring status = L"";

	if (message->tokenNames.size())
	{
		status += L"Activate \"" + message->tokenNames[0].getQualifiedName() + L"\": ";
	}

	status += std::to_wstring(message->tokenIds.size()) + L" ";
	status += (message->tokenIds.size() == 1 ? L"result" : L"results");

	if (fileCount > 0)
	{
		status += L" with " + std::to_wstring(referenceCount) + L" ";
		status += (referenceCount == 1 ? L"reference" : L"references");
		status += L" in " + std::to_wstring(fileCount) + L" ";
		status += (fileCount == 1 ? L"file" : L"files");
	}

	MessageStatus(status).dispatch();
}

void CodeController::handleMessage(MessageActivateTrailEdge* message)
{
	TRACE("trail edge activate");

	saveOrRestoreViewMode(message);

	CodeView::ScrollParams scrollParams(CodeView::ScrollParams::SCROLL_TO_DEFINITION);
	getView()->scrollTo(scrollParams);

	CodeView::CodeParams params;
	params.clearSnippets = true;
	params.showContents = !message->isReplayed();
	params.activeTokenIds = message->edgeIds;

	m_collection = m_storageAccess->getSourceLocationsForTokenIds(params.activeTokenIds);
	showCodeSnippets(getSnippetsForActiveSourceLocations(m_collection.get(), 0), params);
}

void CodeController::handleMessage(MessageChangeFileView* message)
{
	TRACE("code change file");

	CodeView::FileState state;
	switch (message->state)
	{
	case MessageChangeFileView::FILE_MINIMIZED:
		state = CodeView::FILE_MINIMIZED;
		break;

	case MessageChangeFileView::FILE_SNIPPETS:
		state = CodeView::FILE_SNIPPETS;
		break;

	case MessageChangeFileView::FILE_MAXIMIZED:
		state = CodeView::FILE_MAXIMIZED;
		break;
	}

	CodeView* view = getView();
	if (message->viewMode != MessageChangeFileView::VIEW_CURRENT)
	{
		view->setMode(message->viewMode == MessageChangeFileView::VIEW_LIST);
	}

	if (message->needsData && !message->filePath.empty())
	{
		showCodeSnippets(getSnippetsForFileWithState(message->filePath, state), CodeView::CodeParams());
	}

	view->setFileState(message->filePath, state);

	if (!message->isReplayed())
	{
		view->showContents();
	}
}

void CodeController::handleMessage(MessageClearErrorCount* message)
{
	if (getView()->showsErrors())
	{
		clear();
	}
}

void CodeController::handleMessage(MessageDeactivateEdge* message)
{
	if (message->scrollToDefinition)
	{
		CodeView::ScrollParams scrollParams(CodeView::ScrollParams::SCROLL_TO_DEFINITION);
		scrollParams.animated = true;
		scrollParams.ignoreActiveReference = true;
		getView()->scrollTo(scrollParams);
	}
}

void CodeController::handleMessage(MessageFlushUpdates* message)
{
	getView()->showContents();
}

void CodeController::handleMessage(MessageFocusIn* message)
{
	getView()->focusTokenIds(message->tokenIds);
}

void CodeController::handleMessage(MessageFocusOut* message)
{
	getView()->defocusTokenIds();
}

void CodeController::handleMessage(MessageScrollToLine* message)
{
	CodeView::ScrollParams scrollParams(CodeView::ScrollParams::SCROLL_TO_LINE);
	scrollParams.filePath = message->filePath;
	scrollParams.line = message->line;
	getView()->scrollTo(scrollParams);

	getView()->showContents();

	MessageStatus(
		L"Showing source location: " + message->filePath.wstr() + L" : " + std::to_wstring(message->line)
	).dispatch();
}

void CodeController::handleMessage(MessageScrollCode* message)
{
	if (message->isReplayed())
	{
		CodeView::ScrollParams scrollParams(CodeView::ScrollParams::SCROLL_TO_VALUE);
		scrollParams.value = message->value;
		scrollParams.inListMode = message->inListMode;
		getView()->scrollTo(scrollParams);
	}
}

void CodeController::handleMessage(MessageShowErrors* message)
{
	TRACE("code errors");

	saveOrRestoreViewMode(message);

	CodeView* view = getView();
	if (!view->showsErrors() || !message->errorId)
	{
		CodeView::ScrollParams scrollParams(CodeView::ScrollParams::SCROLL_TO_DEFINITION);
		view->scrollTo(scrollParams);

		std::vector<ErrorInfo> errors = m_storageAccess->getErrorsLimited(message->errorIds);
		m_collection = m_storageAccess->getErrorSourceLocations(errors);
		std::vector<CodeSnippetParams> snippets = getSnippetsForCollection(m_collection);

		std::sort(snippets.begin(), snippets.end(), CodeSnippetParams::sortById);

		CodeView::CodeParams params;
		params.clearSnippets = true;
		params.errorInfos = errors;
		params.showContents = !message->isReplayed();
		params.useSingleFileCache = false;

		showCodeSnippets(snippets, params, false);
	}

	if (message->errorId)
	{
		view->showActiveSnippet(std::vector<Id>(1, message->errorId), m_collection, message->isLast());
	}
}

void CodeController::handleMessage(MessageSearchFullText* message)
{
	TRACE("code fulltext");

	saveOrRestoreViewMode(message);

	m_collection = m_storageAccess->getFullTextSearchLocations(message->searchTerm, message->caseSensitive);

	CodeView::ScrollParams scrollParams(CodeView::ScrollParams::SCROLL_TO_DEFINITION);
	getView()->scrollTo(scrollParams);

	CodeView::CodeParams params;
	params.clearSnippets = true;
	params.showContents = !message->isReplayed();
	params.useSingleFileCache = false;

	showCodeSnippets(getSnippetsForCollection(m_collection), params);
}

void CodeController::handleMessage(MessageShowScope* message)
{
	TRACE("code scope");

	std::shared_ptr<SourceLocationCollection> collection =
		m_storageAccess->getSourceLocationsForLocationIds({ message->scopeLocationId });

	SourceLocation* location = collection->getSourceLocationById(message->scopeLocationId);
	if (!location || !location->isScopeLocation() || !location->getOtherLocation())
	{
		LOG_ERROR("MessageShowScope did not contain a valid scope location id");
		return;
	}

	std::vector<CodeSnippetParams> snippets = getSnippetsForFile(collection->getSourceLocationFiles().begin()->second);
	if (snippets.size() != 1)
	{
		LOG_ERROR("MessageShowScope didn't result in one single snippet to be created");
		return;
	}

	snippets[0].insertSnippet = true;

	CodeView::CodeParams params;
	params.showContents = !message->isReplayed();

	addAllSourceLocations(&snippets);
	getView()->showCodeSnippets(snippets, params);
}

CodeView* CodeController::getView() const
{
	return Controller::getView<CodeView>();
}

void CodeController::clear()
{
	getView()->clear();

	m_collection.reset();
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForFileWithState(
	const FilePath& filePath, CodeView::FileState state) const
{
	TRACE();

	std::vector<CodeSnippetParams> snippets;

	switch (state)
	{
	case CodeView::FILE_SNIPPETS:
		{
			std::shared_ptr<SourceLocationFile> file = m_collection->getSourceLocationFileByPath(filePath);
			if (!file)
			{
				return snippets;
			}

			snippets = getSnippetsForFile(file);
		}
		break;

	case CodeView::FILE_MAXIMIZED:
		{
			CodeSnippetParams params;
			params.startLineNumber = 1;
			params.refCount = -1;

			std::shared_ptr<TextAccess> textAccess = m_storageAccess->getFileContent(filePath);
			params.code = textAccess->getText();

			params.modificationTime = m_storageAccess->getFileInfoForFilePath(filePath).lastWriteTime;

			// make a copy of SourceLocationFile so that isWhole flag is different for first snippet adding the file
			// and second snippet adding the content
			params.locationFile =
				std::make_shared<SourceLocationFile>(*m_collection->getSourceLocationFileByPath(filePath).get());
			if (params.locationFile)
			{
				params.locationFile->setIsWhole(true);
			}

			snippets.push_back(params);
		}
		break;

	default:
		break;
	}

	return snippets;
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForActiveSourceLocations(
	const SourceLocationCollection* collection, Id declarationId
) const {
	TRACE();

	std::vector<CodeSnippetParams> snippets;
	collection->forEachSourceLocationFile(
		[&](std::shared_ptr<SourceLocationFile> file) -> void
		{
			bool isDeclarationFile = false;
			bool isDefinitionFile = false;
			file->forEachSourceLocation(
				[&](SourceLocation* location)
				{
					for (Id i : location->getTokenIds())
					{
						if (i == declarationId)
						{
							isDeclarationFile = true;

							if (location->getType() == LOCATION_SCOPE)
							{
								isDefinitionFile = true;
							}

							break;
						}
					}
				}
			);

			CodeSnippetParams params;
			params.locationFile = file;
			params.refCount = file->getUnscopedStartLocationCount();

			params.isDeclaration = isDeclarationFile;
			params.isDefinition = isDefinitionFile;

			params.isCollapsed = true;
			snippets.push_back(params);
		}
	);

	std::sort(snippets.begin(), snippets.end(), CodeSnippetParams::sort);

	addModificationTimes(&snippets);

	return snippets;
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForCollection(
	std::shared_ptr<SourceLocationCollection> collection
) const
{
	TRACE();

	std::vector<CodeSnippetParams> snippets;

	collection->forEachSourceLocationFile(
		[&](std::shared_ptr<SourceLocationFile> file) -> void
		{
			CodeSnippetParams params;
			params.locationFile = file;
			params.refCount = file->getUnscopedStartLocationCount();

			params.isCollapsed = true;
			snippets.push_back(params);
		}
	);

	addModificationTimes(&snippets);

	return snippets;
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForFile(
		std::shared_ptr<SourceLocationFile> activeSourceLocations
) const
{
	TRACE();

	std::shared_ptr<TextAccess> textAccess = m_storageAccess->getFileContent(activeSourceLocations->getFilePath());
	size_t lineCount = textAccess->getLineCount();

	SnippetMerger fileScopedMerger(1, lineCount);
	std::map<int, std::shared_ptr<SnippetMerger>> mergers;

	std::shared_ptr<SourceLocationFile> scopeLocations =
		m_storageAccess->getSourceLocationsOfTypeInFile(activeSourceLocations->getFilePath(), LOCATION_SCOPE);
	activeSourceLocations->forEachStartSourceLocation(
		[&](SourceLocation* location)
		{
			buildMergerHierarchy(location, scopeLocations.get(), fileScopedMerger, mergers);
		}
	);

	std::vector<SnippetMerger::Range> atomicRanges;
	m_storageAccess->getCommentLocationsInFile(activeSourceLocations->getFilePath())->forEachStartSourceLocation(
		[&](SourceLocation* location)
		{
			atomicRanges.push_back(SnippetMerger::Range(
				SnippetMerger::Border(location->getLineNumber(), false),
				SnippetMerger::Border(location->getOtherLocation()->getLineNumber(), false)
			));
		}
	);

	atomicRanges = SnippetMerger::Range::mergeAdjacent(atomicRanges);
	std::deque<SnippetMerger::Range> ranges = fileScopedMerger.merge(atomicRanges);

	const int snippetExpandRange = ApplicationSettings::getInstance()->getCodeSnippetExpandRange();
	std::vector<CodeSnippetParams> snippets;

	for (const SnippetMerger::Range& range: ranges)
	{
		CodeSnippetParams params;
		params.refCount = activeSourceLocations->getUnscopedStartLocationCount();

		params.startLineNumber = std::max<int>(1, range.start.row - (range.start.strong ? 0 : snippetExpandRange));
		params.endLineNumber = std::min<int>(lineCount, range.end.row + (range.end.strong ? 0 : snippetExpandRange));

		params.locationFile = activeSourceLocations->getFilteredByLines(params.startLineNumber, params.endLineNumber);

		if (params.startLineNumber > 1)
		{
			const SourceLocation* location =
				getSourceLocationOfParentScope(params.startLineNumber, scopeLocations.get());
			if (location && location->getTokenIds().size())
			{
				params.title = m_storageAccess->getNameHierarchyForNodeId(location->getTokenIds()[0]).getQualifiedName();
				params.titleId = location->getLocationId();
			}
		}

		if (params.endLineNumber < lineCount)
		{
			const SourceLocation* location =
				getSourceLocationOfParentScope(params.endLineNumber + 1, scopeLocations.get());
			if (location && location->getTokenIds().size())
			{
				params.footer = m_storageAccess->getNameHierarchyForNodeId(location->getTokenIds()[0]).getQualifiedName();
				params.footerId = location->getLocationId();
			}
		}

		if (params.titleId == 0 && params.startLineNumber > 1)
		{
			params.title = activeSourceLocations->getFilePath().wstr();
		}
		else if (params.footerId == 0 && params.endLineNumber < lineCount)
		{
			params.footer = activeSourceLocations->getFilePath().wstr();
		}

		for (const std::string& line: textAccess->getLines(params.startLineNumber, params.endLineNumber))
		{
			params.code += line;
		}

		snippets.push_back(params);
	}

	return snippets;
}

std::shared_ptr<SnippetMerger> CodeController::buildMergerHierarchy(
	const SourceLocation* location,
	const SourceLocationFile* scopeLocations,
	SnippetMerger& fileScopedMerger,
	std::map<int, std::shared_ptr<SnippetMerger>>& mergers
) const
{
	std::shared_ptr<SnippetMerger> currentMerger = std::make_shared<SnippetMerger>(
		location->getStartLocation()->getLineNumber(),
		location->getEndLocation()->getLineNumber()
	);

	const SourceLocation* scopeLocation = getSourceLocationOfParentScope(location->getLineNumber(), scopeLocations);
	if (!scopeLocation)
	{
		fileScopedMerger.addChild(currentMerger);
		return currentMerger;
	}

	std::shared_ptr<SnippetMerger> nextMerger;
	std::map<int, std::shared_ptr<SnippetMerger>>::iterator it = mergers.find(scopeLocation->getLocationId());
	if (it == mergers.end())
	{
		nextMerger = buildMergerHierarchy(scopeLocation, scopeLocations, fileScopedMerger, mergers);
		mergers[scopeLocation->getLocationId()] = nextMerger;
	}
	else
	{
		nextMerger = it->second;
	}
	nextMerger->addChild(currentMerger);
	return currentMerger;
}

const SourceLocation* CodeController::getSourceLocationOfParentScope(
	size_t lineNumber, const SourceLocationFile* scopeLocations) const
{
	const SourceLocation* location = nullptr;

	scopeLocations->forEachStartSourceLocation(
		[&location, lineNumber](SourceLocation* scopeLocation)
		{
			if (scopeLocation->getLineNumber() < lineNumber &&
				scopeLocation->getEndLocation()->getLineNumber() >= lineNumber &&
				(!location || *location < *scopeLocation))
			{
				location = scopeLocation;
			}
		}
	);

	return location;
}

std::vector<std::string> CodeController::getProjectDescription(SourceLocationFile* locationFile) const
{
	Project* currentProject = Application::getInstance()->getCurrentProject().get();
	if (!currentProject)
	{
		return std::vector<std::string>();
	}

	std::string description = currentProject->getDescription();
	if (!description.size())
	{
		return std::vector<std::string>();
	}

	// todo fixme: this split currently prevents the next step from recognizing multi level name hierarchies.
	std::vector<std::string> lines = utility::splitToVector(description, "\\n");
	size_t startLineNumber = 2;

	for (size_t i = 0; i < lines.size(); i++)
	{
		std::string line = "\t" + lines[i];

		line = utility::replace(line, "\\t", "\t");
		line = utility::replace(line, "\\r", "\r");

		size_t pos = 0;
		while (pos != std::string::npos)
		{
			size_t posA = line.find('[', pos);
			size_t posB = line.find(']', posA);

			if (posA == std::string::npos || posB == std::string::npos)
			{
				break;
			}

			std::wstring serializedName = utility::decodeFromUtf8(line.substr(posA + 1, posB - posA - 1));

			NameHierarchy nameHierarchy = NameHierarchy::deserialize(serializedName);
			Id tokenId = m_storageAccess->getNodeIdForNameHierarchy(nameHierarchy);

			std::string nameString = utility::encodeToUtf8(nameHierarchy.getQualifiedName());
			if (tokenId > 0)
			{
				line.replace(posA, posB - posA + 1, nameString);
				locationFile->addSourceLocation(
					LOCATION_TOKEN, 0, {tokenId},
					startLineNumber + i, posA + 1,
					startLineNumber + i, posA + nameString.size()
				);
			}
			else
			{
				line.replace(posA, posB - posA + 1, "[symbol not found (" + nameString + ")]");
			}

			pos = posA + serializedName.size();
		}

		lines[i] = line;
	}

	return lines;
}

void CodeController::expandVisibleSnippets(std::vector<CodeSnippetParams>* snippets, bool useSingleFileCache) const
{
	TRACE();

	if (!snippets->size())
	{
		return;
	}

	bool inListMode = getView()->isInListMode();

	size_t filesToExpand = inListMode ? std::min(int(snippets->size()), 3) : 1;
	CodeView::FileState state = inListMode ? CodeView::FILE_SNIPPETS : CodeView::FILE_MAXIMIZED;

	for (size_t i = 0; i < filesToExpand; i++)
	{
		CodeSnippetParams& oldSnippet = snippets->at(i);
		if (!oldSnippet.isCollapsed || oldSnippet.reduced)
		{
			continue;
		}

		if (useSingleFileCache && !inListMode && getView()->hasSingleFileCached(oldSnippet.locationFile->getFilePath()))
		{
			continue;
		}

		CodeView::FileState fileState = oldSnippet.locationFile->isWhole() ? CodeView::FILE_MAXIMIZED : state;

		std::vector<CodeSnippetParams> newSnippets =
			getSnippetsForFileWithState(oldSnippet.locationFile->getFilePath(), fileState);
		if (!newSnippets.size())
		{
			continue;
		}

		for (CodeSnippetParams& newSnippet : newSnippets)
		{
			newSnippet.isDeclaration = oldSnippet.isDeclaration;
			newSnippet.isDefinition = oldSnippet.isDefinition;

			newSnippet.isCollapsed = false;
		}

		snippets->insert(snippets->end(), newSnippets.begin(), newSnippets.end());
	}
}

void CodeController::addAllSourceLocations(std::vector<CodeSnippetParams>* snippets) const
{
	TRACE();

	for (CodeSnippetParams& snippet : *snippets)
	{
		if (!snippet.locationFile || snippet.isCollapsed || snippet.reduced)
		{
			continue;
		}

		std::shared_ptr<SourceLocationFile> file;

		if (snippet.locationFile->isWhole())
		{
			file = m_storageAccess->getSourceLocationsForFile(snippet.locationFile->getFilePath());
		}
		else
		{
			file = m_storageAccess->getSourceLocationsForLinesInFile(
				snippet.locationFile->getFilePath(), snippet.startLineNumber, snippet.endLineNumber);
		}

		if (file)
		{
			file->copySourceLocations(snippet.locationFile);
			snippet.locationFile = file;
		}
	}
}

void CodeController::addModificationTimes(std::vector<CodeSnippetParams>* snippets) const
{
	TRACE();

	std::vector<FilePath> filePaths;
	for (const CodeSnippetParams& snippet : *snippets)
	{
		filePaths.push_back(snippet.locationFile->getFilePath());
	}
	std::vector<FileInfo> fileInfos = m_storageAccess->getFileInfosForFilePaths(filePaths);

	std::map<FilePath, FileInfo> fileInfoMap;
	for (FileInfo& fileInfo : fileInfos)
	{
		fileInfoMap.emplace(fileInfo.path, fileInfo);
	}

	for (CodeSnippetParams& snippet : *snippets)
	{
		snippet.modificationTime = fileInfoMap[snippet.locationFile->getFilePath()].lastWriteTime;
	}
}

void CodeController::saveOrRestoreViewMode(MessageBase* message)
{
	if (message->isReplayed())
	{
		auto it = m_messageIdToViewModeMap.find(message->getId());
		if (it != m_messageIdToViewModeMap.end())
		{
			getView()->setMode(it->second);
		}
	}
	else
	{
		m_messageIdToViewModeMap.emplace(message->getId(), getView()->isInListMode());
	}
}

void CodeController::showCodeSnippets(
	std::vector<CodeSnippetParams> snippets, const CodeView::CodeParams params, bool addSourceLocations)
{
	expandVisibleSnippets(&snippets, params.useSingleFileCache);

	CodeView* view = getView();
	view->showCodeSnippets(snippets, params);

	if (addSourceLocations)
	{
		addAllSourceLocations(&snippets);
		view->updateCodeSnippets(snippets);
	}
}
