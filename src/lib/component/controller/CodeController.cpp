#include "component/controller/CodeController.h"

#include <memory>

#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/tracing.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "data/access/StorageAccess.h"
#include "data/location/SourceLocation.h"
#include "data/location/SourceLocationCollection.h"
#include "data/location/SourceLocationFile.h"
#include "settings/ApplicationSettings.h"
#include "Application.h"

CodeController::CodeController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
	, m_scrollToDefinition(false)
	, m_scrollToValue(-1)
	, m_scrollToLine(0)
{
}

CodeController::~CodeController()
{
}

const uint CodeController::s_lineRadius = 2;

void CodeController::handleMessage(MessageActivateAll* message)
{
	TRACE("code all");

	clear();

	Project* currentProject = Application::getInstance()->getCurrentProject().get();
	if (!currentProject)
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

	CodeView* view = getView();
	view->showCodeSnippets(std::vector<CodeSnippetParams>(1, statsSnippet), std::vector<Id>(), true);

	showContents(message);
}

void CodeController::handleMessage(MessageActivateLocalSymbols* message)
{
	CodeView* view = getView();
	view->showActiveLocalSymbolIds(message->symbolIds);
}

void CodeController::handleMessage(MessageActivateTokens* message)
{
	TRACE("code activate");

	CodeView* view = getView();

	std::vector<Id> activeTokenIds = message->tokenIds;
	if (!activeTokenIds.size())
	{
		view->clear();
		return;
	}

	if (!message->keepContent())
	{
		view->clearCodeSnippets();
	}

	Id declarationId = 0; // 0 means that no token is found.
	if (!message->isAggregation)
	{
		activeTokenIds = m_storageAccess->getActiveTokenIdsForId(activeTokenIds[0], &declarationId);
	}

	if (message->isEdge)
	{
		std::shared_ptr<SourceLocationCollection> collection = m_storageAccess->getSourceLocationsForTokenIds(activeTokenIds);
		view->showActiveSnippet(activeTokenIds, collection, message->isLast());
	}
	else if (message->keepContent())
	{
		view->showActiveTokenIds(activeTokenIds);
		if (m_scrollToDefinition)
		{
			getView()->scrollToDefinition(true);
			m_scrollToDefinition = false;
		}
	}
	else
	{
		m_collection = m_storageAccess->getSourceLocationsForTokenIds(activeTokenIds);
		view->showCodeSnippets(
			getSnippetsForActiveSourceLocations(m_collection.get(), declarationId),
			activeTokenIds,
			!message->isReplayed() || message->isReplayCleared()
		);
		m_scrollToDefinition = !message->isReplayed() || message->isReplayCleared();

		size_t fileCount = m_collection->getSourceLocationFileCount();
		size_t referenceCount = m_collection->getSourceLocationCount();

		std::stringstream ss;

		if (message->tokenNames.size())
		{
			ss << "Activate \"" << message->tokenNames[0].getQualifiedName() << "\": ";
		}

		ss << message->tokenIds.size() << ' ';
		ss << (message->tokenIds.size() == 1 ? "result" : "results");

		if (fileCount > 0)
		{
			ss << " with " << referenceCount << ' ';
			ss << (referenceCount == 1 ? "reference" : "references");
			ss << " in " << fileCount << ' ';
			ss << (fileCount == 1 ? "file" : "files");
		}

		MessageStatus(ss.str()).dispatch();
	}

	showContents(message);
}

void CodeController::handleMessage(MessageChangeFileView* message)
{
	TRACE("code change file");

	if (!m_collection)
	{
		return;
	}

	CodeView* view = getView();
	bool inListMode = view->isInListMode();

	MessageChangeFileView::FileState state = message->state;
	if (state == MessageChangeFileView::FILE_DEFAULT_FOR_MODE)
	{
		state = inListMode ? MessageChangeFileView::FILE_SNIPPETS : MessageChangeFileView::FILE_MAXIMIZED;
	}

	switch (state)
	{
	case MessageChangeFileView::FILE_MINIMIZED:
		view->setFileState(message->filePath, CodeView::FILE_MINIMIZED);
		break;

	case MessageChangeFileView::FILE_SNIPPETS:
		if (message->needsData && inListMode)
		{
			std::shared_ptr<SourceLocationFile> file = m_collection->getSourceLocationFileByPath(message->filePath);
			if (!file)
			{
				return;
			}

			if (message->showErrors)
			{
				file->setIsWhole(false);
			}

			view->addCodeSnippets(getSnippetsForFile(file, !message->showErrors), false);
		}
		view->setFileState(message->filePath, CodeView::FILE_SNIPPETS);
		break;

	case MessageChangeFileView::FILE_MAXIMIZED:
		if (message->needsData)
		{
			CodeSnippetParams params;
			params.startLineNumber = 1;
			params.refCount = -1;

			std::shared_ptr<TextAccess> textAccess = m_storageAccess->getFileContent(message->filePath);
			params.code = textAccess->getText();

			params.modificationTime = m_storageAccess->getFileInfoForFilePath(message->filePath).lastWriteTime;

			if (message->showErrors)
			{
				params.locationFile = m_collection->getSourceLocationFileByPath(message->filePath);
				if (!params.locationFile)
				{
					return;
				}
				params.locationFile->setIsWhole(true);
			}
			else
			{
				std::shared_ptr<SourceLocationFile> file =
					m_storageAccess->getSourceLocationsForFile(message->filePath.str());

				SourceLocationFile* activeLocations = m_collection->getSourceLocationFileByPath(message->filePath).get();
				if (activeLocations)
				{
					activeLocations->forEachSourceLocation(
						[&file](SourceLocation* location)
						{
							file->addSourceLocationCopy(location);
						}
					);
				}

				params.locationFile = file;
			}

			getView()->addCodeSnippets(std::vector<CodeSnippetParams>(1, params), false);
		}

		view->setFileState(message->filePath, CodeView::FILE_MAXIMIZED);
		break;
	default:
		break;
	}

	showContents(message);
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
		m_scrollToDefinition = true;
	}
}

void CodeController::handleMessage(MessageFlushUpdates* message)
{
	MessageCodeViewExpandedInitialFiles* msgPtr = nullptr;
	handleMessage(msgPtr);

	showContents(message);
}

void CodeController::handleMessage(MessageFocusIn* message)
{
	getView()->focusTokenIds(message->tokenIds);
}

void CodeController::handleMessage(MessageFocusOut* message)
{
	getView()->defocusTokenIds();
}

void CodeController::handleMessage(MessageCodeViewExpandedInitialFiles* message)
{
	if (m_scrollToDefinition || (message && message->scrollToDefinition))
	{
		getView()->scrollToDefinition(false);
		m_scrollToDefinition = false;
	}

	if (m_scrollToValue != -1)
	{
		getView()->scrollToValue(m_scrollToValue, m_scrollInListMode);
		m_scrollToValue = -1;
	}

	if (m_scrollToLine)
	{
		getView()->scrollToLine(m_scrollToFilePath, m_scrollToLine);
		m_scrollToLine = 0;
	}
}

void CodeController::handleMessage(MessageScrollToLine* message)
{
	m_scrollToFilePath = message->filePath;
	m_scrollToLine = message->line;

	if (message->isModified)
	{
		MessageStatus(
			"Showing source location: " + message->filePath.str()
			+ " : " + std::to_string(message->line) + ". The file was modified, please refresh.",
			true
		).dispatch();
	}
	else
	{
		MessageStatus(
			"Showing source location: " + message->filePath.str() + " : " + std::to_string(message->line)
		).dispatch();
	}
}

void CodeController::handleMessage(MessageScrollCode* message)
{
	if (message->isReplayed())
	{
		m_scrollToValue = message->value;
		m_scrollInListMode = message->inListMode;
	}
}

void CodeController::handleMessage(MessageShowErrors* message)
{
	TRACE("code errors");

	CodeView* view = getView();
	if (!view->showsErrors() || !message->errorId)
	{
		std::vector<ErrorInfo> errors;
		m_collection = m_storageAccess->getErrorSourceLocations(&errors);
		std::vector<CodeSnippetParams> snippets = getSnippetsForCollection(m_collection);

		view->clear();
		view->setErrorInfos(errors);
		view->showCodeSnippets(
			snippets, std::vector<Id>(), !message->errorId && (!message->isReplayed() || message->isReplayCleared()));

		showContents(message);
	}

	if (message->errorId)
	{
		view->showActiveSnippet(std::vector<Id>(1, message->errorId), m_collection, true);
	}
}

void CodeController::handleMessage(MessageSearchFullText* message)
{
	TRACE("code fulltext");

	CodeView* view = getView();
	view->clear();

	m_collection = m_storageAccess->getFullTextSearchLocations(message->searchTerm, message->caseSensitive);
	view->showCodeSnippets(
		getSnippetsForCollection(m_collection, true), std::vector<Id>(), !message->isReplayed() || message->isReplayCleared());

	showContents(message);
}

void CodeController::handleMessage(MessageShowScope* message)
{
	TRACE("code scope");

	std::shared_ptr<SourceLocationCollection> collection =
		m_storageAccess->getSourceLocationsForLocationIds({message->scopeLocationId});

	SourceLocation* location = collection->getSourceLocationById(message->scopeLocationId);
	if (!location || !location->isScopeLocation() || !location->getOtherLocation())
	{
		LOG_ERROR("MessageShowScope did not contain a valid scope location id");
		return;
	}

	std::vector<CodeSnippetParams> snippets =
		getSnippetsForFile(collection->getSourceLocationFiles().begin()->second, true);
	if (snippets.size() != 1)
	{
		LOG_ERROR("MessageShowScope didn't result in one single snippet to be created");
		return;
	}

	if (message->showErrors)
	{
		snippets[0].locationFile = m_collection->getSourceLocationFileByPath(snippets[0].locationFile->getFilePath());
	}
	else
	{
		SourceLocationFile* activeLocations =
			m_collection->getSourceLocationFileByPath(snippets[0].locationFile->getFilePath()).get();
		if (activeLocations)
		{
			std::shared_ptr<SourceLocationFile> file = snippets[0].locationFile;
			activeLocations->forEachSourceLocation(
				[&file](SourceLocation* location)
				{
					file->addSourceLocationCopy(location);
				}
			);
		}
	}

	getView()->addCodeSnippets(snippets, true);

	showContents(message);
}

CodeView* CodeController::getView()
{
	return Controller::getView<CodeView>();
}

void CodeController::clear()
{
	getView()->clear();

	m_collection.reset();
}

void CodeController::showContents(MessageBase* message)
{
	if (!message->isReplayed())
	{
		getView()->showContents();
	}
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

	addModificationTimes(snippets);

	return snippets;
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForCollection(
	std::shared_ptr<SourceLocationCollection> collection, bool addSourceLocations
) const
{
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

	addModificationTimes(snippets);

	return snippets;
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForFile(
		std::shared_ptr<SourceLocationFile> activeSourceLocations, bool addSourceLocations
) const
{
	TRACE();

	std::shared_ptr<SourceLocationFile> fileLocations =
		m_storageAccess->getSourceLocationsForFile(activeSourceLocations->getFilePath());
	std::shared_ptr<SourceLocationFile> scopeLocations = fileLocations->getFilteredByType(LOCATION_SCOPE);

	std::shared_ptr<TextAccess> textAccess = m_storageAccess->getFileContent(activeSourceLocations->getFilePath());
	std::deque<SnippetMerger::Range> ranges;

	if (activeSourceLocations->isWhole())
	{
		ranges.push_back(SnippetMerger::Range(
			SnippetMerger::Border(1, true),
			SnippetMerger::Border(textAccess->getLineCount(), true)
		));
	}
	else
	{
		SnippetMerger fileScopedMerger(1, textAccess->getLineCount());
		std::map<int, std::shared_ptr<SnippetMerger>> mergers;
		activeSourceLocations->forEachStartSourceLocation(
			[&](SourceLocation* location)
			{
				buildMergerHierarchy(location, scopeLocations, fileScopedMerger, mergers);
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

		ranges = fileScopedMerger.merge(atomicRanges);
	}

	const int snippetExpandRange = ApplicationSettings::getInstance()->getCodeSnippetExpandRange();
	std::vector<CodeSnippetParams> snippets;

	for (const SnippetMerger::Range& range: ranges)
	{
		CodeSnippetParams params;
		params.refCount = activeSourceLocations->getUnscopedStartLocationCount();

		params.startLineNumber = std::max<int>(1, range.start.row - (range.start.strong ? 0 : snippetExpandRange));
		params.endLineNumber =
			std::min<int>(textAccess->getLineCount(), range.end.row + (range.end.strong ? 0 : snippetExpandRange));

		params.locationFile = activeSourceLocations->getFilteredByLines(params.startLineNumber, params.endLineNumber);
		params.titleId = 0;
		params.footerId = 0;

		std::shared_ptr<SourceLocationFile> tempFile =
			fileLocations->getFilteredByLines(params.startLineNumber, params.endLineNumber);

		const SourceLocation* firstSourceLocation =
			tempFile->getSourceLocations().size() ? tempFile->getSourceLocations().begin()->get() : nullptr;

		if (firstSourceLocation)
		{
			// this SourceLocationFile only contains a single StartSourceLocation.
			getSourceLocationOfParentScope(firstSourceLocation, scopeLocations)->forEachStartSourceLocation(
				[&](SourceLocation* location)
				{
					if (location->getTokenIds().size())
					{
						params.title = m_storageAccess->getNameHierarchyForNodeId(location->getTokenIds()[0]).getQualifiedName();
						params.titleId = location->getLocationId();
					}
				}
			);
		}

		if (!activeSourceLocations->isWhole() && params.titleId == 0)
		{
			params.title = activeSourceLocations->getFilePath().str();
		}

		const SourceLocation* lastSourceLocation =
			tempFile->getSourceLocations().size() ? tempFile->getSourceLocations().rbegin()->get() : nullptr;
		if (lastSourceLocation)
		{
			// this SourceLocationFile only contains a single StartSourceLocation.
			getSourceLocationOfParentScope(lastSourceLocation, scopeLocations)->forEachStartSourceLocation(
				[&](SourceLocation* location)
				{
					if (location->getTokenIds().size())
					{
						params.footer = m_storageAccess->getNameHierarchyForNodeId(location->getTokenIds()[0]).getQualifiedName();
						params.footerId = location->getLocationId();
					}
				}
			);
		}

		for (const std::string& line: textAccess->getLines(params.startLineNumber, params.endLineNumber))
		{
			params.code += line;
		}

		snippets.push_back(params);
	}

	if (addSourceLocations && !activeSourceLocations->isWhole())
	{
		for (CodeSnippetParams& params : snippets)
		{
			std::shared_ptr<SourceLocationFile> lines =
				fileLocations->getFilteredByLines(params.startLineNumber, params.endLineNumber);

			params.locationFile->forEachSourceLocation(
				[&lines](SourceLocation* location)
				{
					lines->addSourceLocationCopy(location);
				}
			);

			params.locationFile = lines;
		}
	}

	return snippets;
}

std::shared_ptr<SnippetMerger> CodeController::buildMergerHierarchy(
	SourceLocation* location,
	std::shared_ptr<SourceLocationFile> scopeLocations,
	SnippetMerger& fileScopedMerger,
	std::map<int, std::shared_ptr<SnippetMerger>>& mergers
) const
{
	std::shared_ptr<SnippetMerger> currentMerger = std::make_shared<SnippetMerger>(
		location->getStartLocation()->getLineNumber(),
		location->getEndLocation()->getLineNumber()
	);

	std::shared_ptr<SourceLocationFile> locationFile = getSourceLocationOfParentScope(location, scopeLocations);
	if (locationFile->getSourceLocationCount() == 0)
	{
		fileScopedMerger.addChild(currentMerger);
		return currentMerger;
	}

	std::shared_ptr<SnippetMerger> nextMerger;
	locationFile->forEachStartSourceLocation( // contains just 1 start location
		[&](SourceLocation* scopeLocation)
		{
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
		}
	);
	nextMerger->addChild(currentMerger);
	return currentMerger;
}

std::shared_ptr<SourceLocationFile> CodeController::getSourceLocationOfParentScope(
	const SourceLocation* location,
	std::shared_ptr<SourceLocationFile> scopeLocations
) const
{
	const SourceLocation* parent = nullptr;

	scopeLocations->forEachStartSourceLocation(
		[&](SourceLocation* scopeLocation) -> void
		{
			if (location->getStartLocation() && *scopeLocation == *location->getStartLocation() &&
				location->getEndLocation() && *scopeLocation->getEndLocation() == *location->getEndLocation())
			{
				return;
			}

			if (!(*scopeLocation > *location) &&
				!(*scopeLocation->getEndLocation() < *location) &&
				// since scopeLocation is a start location the > location indicates the scope
				// that is closer to the child.
				(!parent || *scopeLocation > *parent))
			{
				parent = scopeLocation;
			}
		}
	);

	std::shared_ptr<SourceLocationFile> file = std::make_shared<SourceLocationFile>(location->getFilePath(), false, false);
	if (parent)
	{
		file->addSourceLocationCopy(parent);
		file->addSourceLocationCopy(parent->getOtherLocation());
	}
	return file;
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

			std::string serializedName = line.substr(posA + 1, posB - posA - 1);

			NameHierarchy nameHierarchy = NameHierarchy::deserialize(serializedName);
			Id tokenId = m_storageAccess->getNodeIdForNameHierarchy(nameHierarchy);

			std::string nameString = nameHierarchy.getQualifiedName();
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

void CodeController::addModificationTimes(std::vector<CodeSnippetParams>& snippets) const
{
	TRACE();

	std::vector<FilePath> filePaths;
	for (const CodeSnippetParams& snippet : snippets)
	{
		filePaths.push_back(snippet.locationFile->getFilePath());
	}
	std::vector<FileInfo> fileInfos = m_storageAccess->getFileInfosForFilePaths(filePaths);

	std::map<FilePath, FileInfo> fileInfoMap;
	for (FileInfo& fileInfo : fileInfos)
	{
		fileInfoMap.emplace(fileInfo.path, fileInfo);
	}

	for (CodeSnippetParams& snippet : snippets)
	{
		snippet.modificationTime = fileInfoMap[snippet.locationFile->getFilePath()].lastWriteTime;
	}
}
