#include "CodeController.h"

#include <memory>

#include "StorageAccess.h"
#include "SourceLocation.h"
#include "SourceLocationCollection.h"
#include "SourceLocationFile.h"
#include "ApplicationSettings.h"
#include "FileInfo.h"
#include "logging.h"
#include "MessageMoveIDECursor.h"
#include "MessageStatus.h"
#include "TextAccess.h"
#include "tracing.h"
#include "utilityString.h"
#include "Application.h"

CodeController::CodeController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

Id CodeController::getSchedulerId() const
{
	return Controller::getTabId();
}

const uint CodeController::s_lineRadius = 2;

void CodeController::handleMessage(MessageActivateAll* message)
{
	TRACE("code all");

	saveOrRestoreViewMode(message);

	Project* currentProject = Application::getInstance()->getCurrentProject().get();
	if (!currentProject || message->acceptedNodeTypes != NodeTypeSet::all())
	{
		clear();
		return;
	}

	CodeSnippetParams statsSnippet;
	statsSnippet.title = currentProject->getProjectSettingsFilePath().withoutExtension().fileName();

	statsSnippet.startLineNumber = 1;
	statsSnippet.endLineNumber = 1;

	statsSnippet.reduced = true;

	statsSnippet.locationFile = std::make_shared<SourceLocationFile>(FilePath(), L"", true, true, true);

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
	ss << "\t" + std::to_string(stats.edgeCount) + " references\n";
	ss << "\n";
	ss << "\t" + std::to_string(errorCount.total) + " errors (" + std::to_string(errorCount.fatal) + " fatal)\n";
	ss << "\n";

	if (errorCount.fatal)
	{
		ss << "\tWarning: Your project has fatal errors, which cause\n";
		ss << "\t  a lot of missing information in affected files.\n";
		ss << "\t  Try to resolve them!\n";
		ss << "\n";
	}

	statsSnippet.code = ss.str();

	CodeView::CodeParams params;
	params.clearSnippets = true;
	params.showContents = !message->isReplayed();
	showCodeSnippets({ statsSnippet }, params);
}

void CodeController::handleMessage(MessageActivateErrors* message)
{
	TRACE("code errors");

	saveOrRestoreViewMode(message);

	CodeView* view = getView();

	CodeView::ScrollParams scrollParams(CodeView::ScrollParams::SCROLL_TO_DEFINITION);
	view->scrollTo(scrollParams);

	std::vector<ErrorInfo> errors;
	if (message->file.empty())
	{
		errors = m_storageAccess->getErrorsLimited(message->filter);
	}
	else
	{
		errors = m_storageAccess->getErrorsForFileLimited(message->filter, message->file);
	}

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

void CodeController::handleMessage(MessageActivateFullTextSearch* message)
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

void CodeController::handleMessage(MessageActivateLegend* message)
{
	clear();
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
		std::vector<Id> activeTokenIds;
		for (Id tokenId : params.activeTokenIds)
		{
			utility::append(activeTokenIds, m_storageAccess->getActiveTokenIdsForId(tokenId, &declarationId));
		}
		params.activeTokenIds = activeTokenIds;
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

	std::wstring status;
	for (const SearchMatch& match : message->getSearchMatches())
	{
		status += L"Activate \"" + match.name + L"\": ";
		break;
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
		view->scrollTo(CodeView::ScrollParams());
		showCodeSnippets(getSnippetsForFileWithState(message->filePath, state), CodeView::CodeParams(), !message->showErrors);
	}

	view->setFileState(message->filePath, state);

	if (!message->isReplayed())
	{
		view->showContents();
	}
}

void CodeController::handleMessage(MessageCodeShowDefinition* message)
{
	TRACE("code show definition");

	Id nodeId = message->nodeId;

	std::shared_ptr<SourceLocationCollection> collection = m_storageAccess->getSourceLocationsForTokenIds({ nodeId });
	if (!collection->getSourceLocationFileCount())
	{
		LOG_ERROR("MessageCodeShowDefinition did not contain a nodeId with location files.");
		return;
	}

	size_t lineNumber = 1;
	size_t columnNumber = 1;
	FilePath filePath;

	// use first scope location for nodeId, otherwise first location
	if (collection->getSourceLocationCount())
	{
		std::shared_ptr<SourceLocationCollection> filteredCollection = std::make_shared<SourceLocationCollection>();
		bool addedLocation = false;

		collection->forEachSourceLocation(
			[&](SourceLocation* location)
			{
				if (addedLocation || !location->isStartLocation())
				{
					return;
				}

				if (location->isScopeLocation())
				{
					filteredCollection->addSourceLocationCopy(location);
					filteredCollection->addSourceLocationCopy(location->getEndLocation());

					filePath = location->getFilePath();
					lineNumber = location->getLineNumber();
					columnNumber = location->getColumnNumber();
					addedLocation = true;
					return;
				}
			}
		);

		if (!addedLocation)
		{
			SourceLocation* location = collection->getSourceLocationFiles().begin()->second->getSourceLocations().begin()->get();
			filteredCollection->addSourceLocationCopy(location);
			filteredCollection->addSourceLocationCopy(location->getOtherLocation());

			filePath = location->getFilePath();
			lineNumber = location->getStartLocation()->getLineNumber();
			columnNumber = location->getStartLocation()->getColumnNumber();
		}

		collection = filteredCollection;
	}
	else // otherwise first file
	{
		filePath = collection->getSourceLocationFiles().begin()->second->getFilePath();
	}

	if (message->inIDE)
	{
		MessageMoveIDECursor(filePath, lineNumber, columnNumber).dispatch();
		return;
	}

	std::vector<CodeSnippetParams> snippets = getSnippetsForFile(collection->getSourceLocationFiles().begin()->second);
	if (snippets.size() != 1)
	{
		LOG_ERROR("MessageCodeShowDefinition snippet count is not 1");
		return;
	}

	snippets[0].insertSnippet = true;
	m_collection->addSourceLocationCopies(collection.get());

	saveOrRestoreViewMode(message);

	CodeView* view = getView();
	CodeView::ScrollParams scrollParams(CodeView::ScrollParams::SCROLL_TO_LINE);
	scrollParams.filePath = filePath;
	scrollParams.line = lineNumber;
	view->scrollTo(scrollParams);

	CodeView::CodeParams params;
	params.showContents = !message->isReplayed();

	addAllSourceLocations(&snippets);
	getView()->showCodeSnippets(snippets, params);
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

void CodeController::handleMessage(MessageErrorCountClear* message)
{
	if (getView()->showsErrors())
	{
		clear();
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

void CodeController::handleMessage(MessageShowError* message)
{
	CodeView* view = getView();
	if (view->showsErrors())
	{
		view->showActiveSnippet({ message->errorId }, m_collection, message->isLast());
	}
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

	m_collection = std::make_shared<SourceLocationCollection>();
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForFileWithState(
	const FilePath& filePath, CodeView::FileState state) const
{
	TRACE();

	std::shared_ptr<SourceLocationFile> file = m_collection->getSourceLocationFileByPath(filePath);
	if (!file)
	{
		return {};
	}

	std::vector<CodeSnippetParams> snippets;

	switch (state)
	{
	case CodeView::FILE_SNIPPETS:
		snippets = getSnippetsForFile(file);
		break;

	case CodeView::FILE_MAXIMIZED:
		{
			CodeSnippetParams params;
			params.startLineNumber = 1;
			params.refCount = -1;

			bool showsErrors = false;
			if (m_collection->getSourceLocationFiles().size())
			{
				std::shared_ptr<SourceLocationFile> file = m_collection->getSourceLocationFiles().begin()->second;
				if (file->getSourceLocations().size())
				{
					showsErrors = (*file->getSourceLocations().begin())->getType() == LOCATION_ERROR;
				}
			}

			std::shared_ptr<TextAccess> textAccess = m_storageAccess->getFileContent(filePath, showsErrors);
			params.code = textAccess->getText();

			params.modificationTime = m_storageAccess->getFileInfoForFilePath(filePath).lastWriteTime;

			// make a copy of SourceLocationFile so that isWhole flag is different for first snippet adding the file
			// and second snippet adding the content
			params.locationFile = std::make_shared<SourceLocationFile>(*file.get());
			params.locationFile->setIsWhole(true);

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

	bool showsErrors = false;
	if (activeSourceLocations->getSourceLocations().size())
	{
		showsErrors = (*activeSourceLocations->getSourceLocations().begin())->getType() == LOCATION_ERROR;
	}

	std::shared_ptr<TextAccess> textAccess =
		m_storageAccess->getFileContent(activeSourceLocations->getFilePath(), showsErrors);
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
	std::shared_ptr<SourceLocationFile> commentLocations =
		m_storageAccess->getSourceLocationsOfTypeInFile(activeSourceLocations->getFilePath(), LOCATION_COMMENT);
	commentLocations->forEachStartSourceLocation(
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
		std::string line = "\t" + utility::trim(lines[i]);

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
					LOCATION_TOKEN, 0, { tokenId },
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
