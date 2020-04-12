#include "CodeController.h"

#include <memory>

#include "Application.h"
#include "ApplicationSettings.h"
#include "FileInfo.h"
#include "MessageFocusView.h"
#include "MessageMoveIDECursor.h"
#include "MessageShowError.h"
#include "MessageStatus.h"
#include "SourceLocation.h"
#include "SourceLocationCollection.h"
#include "SourceLocationFile.h"
#include "StorageAccess.h"
#include "TextAccess.h"
#include "logging.h"
#include "tracing.h"
#include "utility.h"
#include "utilityString.h"

CodeController::CodeController(StorageAccess* storageAccess): m_storageAccess(storageAccess) {}

Id CodeController::getSchedulerId() const
{
	return Controller::getTabId();
}

void CodeController::handleMessage(MessageActivateErrors* message)
{
	TRACE("code errors");

	saveOrRestoreViewMode(message);

	CodeView* view = getView();

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

	m_files = getFilesForCollection(m_collection);
	std::sort(m_files.begin(), m_files.end(), CodeFileParams::sortById);

	CodeView::CodeParams params;
	params.clearSnippets = true;
	params.useSingleFileCache = false;
	params.errorInfos = errors;

	createReferences();
	expandVisibleFiles(params.useSingleFileCache);
	showFiles(params, firstReferenceScrollParams(), !message->isReplayed());
}

void CodeController::handleMessage(MessageActivateFullTextSearch* message)
{
	TRACE("code fulltext");

	saveOrRestoreViewMode(message);

	m_collection = m_storageAccess->getFullTextSearchLocations(
		message->searchTerm, message->caseSensitive);

	CodeView::CodeParams params;
	params.clearSnippets = true;
	params.useSingleFileCache = false;

	m_files = getFilesForCollection(m_collection);
	createReferences();
	expandVisibleFiles(params.useSingleFileCache);
	showFiles(params, firstReferenceScrollParams(), !message->isReplayed());
}

void CodeController::handleMessage(MessageActivateLegend* message)
{
	clear();
}

void CodeController::handleMessage(MessageActivateLocalSymbols* message)
{
	createLocalReferences(utility::toSet(message->symbolIds));
	m_codeParams.activeLocalSymbolIds = message->symbolIds;
	m_codeParams.activeLocalSymbolType = LOCATION_LOCAL_SYMBOL;
	m_codeParams.currentActiveLocalLocationIds.clear();
	m_codeParams.locationIdToFocus = 0;
	showFiles(m_codeParams, CodeScrollParams(), !message->isReplayed());
}

void CodeController::handleMessage(MessageActivateOverview* message)
{
	TRACE("code all");

	saveOrRestoreViewMode(message);
	clearReferences();

	std::shared_ptr<const Project> currentProject = Application::getInstance()->getCurrentProject();
	if (!currentProject || message->acceptedNodeTypes != NodeTypeSet::all())
	{
		clear();
		return;
	}

	CodeSnippetParams statsSnippet;
	statsSnippet.hasAllSourceLocations = true;
	statsSnippet.title = currentProject->getProjectSettingsFilePath().withoutExtension().fileName();
	statsSnippet.isOverview = true;

	statsSnippet.startLineNumber = 1;
	statsSnippet.endLineNumber = 1;

	statsSnippet.locationFile = std::make_shared<SourceLocationFile>(
		FilePath(), L"", true, true, true);

	std::vector<std::string> description = getProjectDescription(statsSnippet.locationFile.get());

	std::stringstream ss;

	if (description.size())
	{
		ss << "\n";
		for (const std::string& line: description)
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
	ss << (stats.completedFileCount != stats.fileCount
			   ? " (" + std::to_string(stats.completedFileCount) + " complete)"
			   : "") +
			"\n";
	ss << "\t" + std::to_string(stats.fileLOCCount) + " lines of code\n";
	ss << "\n";
	ss << "\t" + std::to_string(stats.nodeCount) + " symbols\n";
	ss << "\t" + std::to_string(stats.edgeCount) + " references\n";
	ss << "\n";
	ss << "\t" + std::to_string(errorCount.total) + " errors (" + std::to_string(errorCount.fatal) +
			" fatal)\n";
	ss << "\n";

	if (errorCount.fatal)
	{
		ss << "\tWarning: Your project has fatal errors, which cause\n";
		ss << "\t  a lot of missing information in affected files.\n";
		ss << "\t  Try to resolve them!\n";
		ss << "\n";
	}

	statsSnippet.code = ss.str();

	CodeFileParams file;
	file.isMinimized = false;
	file.locationFile = statsSnippet.locationFile;
	file.snippetParams.push_back(statsSnippet);
	file.fileParams = std::make_shared<CodeSnippetParams>(statsSnippet);

	m_currentFilePath = file.locationFile->getFilePath();
	m_files = {file};

	CodeView::CodeParams params;
	params.clearSnippets = true;
	params.useSingleFileCache = false;

	showFiles(params, CodeScrollParams(), !message->isReplayed());
}

void CodeController::handleMessage(MessageActivateTokens* message)
{
	TRACE("code activate");

	saveOrRestoreViewMode(message);

	CodeView* view = getView();
	if (!message->tokenIds.size())
	{
		view->clear();
		return;
	}

	CodeView::CodeParams params;
	params.activeTokenIds = message->tokenIds;
	params.clearSnippets = true;

	Id declarationId = 0;	 // 0 means that no token is found.
	if (!message->isAggregation)
	{
		std::vector<Id> activeTokenIds;
		for (Id tokenId: params.activeTokenIds)
		{
			utility::append(
				activeTokenIds, m_storageAccess->getActiveTokenIdsForId(tokenId, &declarationId));
		}
		params.activeTokenIds = activeTokenIds;
	}

	if (message->isEdge && params.activeTokenIds.size() == 1)
	{
		showFirstActiveReference(params.activeTokenIds[0], !message->isReplayed());
		return;
	}

	m_collection = m_storageAccess->getSourceLocationsForTokenIds(params.activeTokenIds);

	m_files = getFilesForActiveSourceLocations(m_collection.get(), declarationId);
	createReferences();
	expandVisibleFiles(params.useSingleFileCache);
	showFiles(params, definitionReferenceScrollParams(params.activeTokenIds), !message->isReplayed());

	// send status message
	{
		size_t fileCount = m_collection->getSourceLocationFileCount();
		size_t referenceCount = m_collection->getSourceLocationCount();

		std::wstring status;
		for (const SearchMatch& match: message->getSearchMatches())
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
}

void CodeController::handleMessage(MessageActivateTrail* message)
{
	if (message->custom)
	{
		getView()->clear();
		Id nodeId = {message->originId ? message->originId : message->targetId};
		MessageCodeShowDefinition msg(nodeId);
		handleMessage(&msg);
	}
}

void CodeController::handleMessage(MessageActivateTrailEdge* message)
{
	TRACE("trail edge activate");

	saveOrRestoreViewMode(message);

	m_codeParams.activeTokenIds = message->edgeIds;

	m_collection = m_storageAccess->getSourceLocationsForTokenIds(m_codeParams.activeTokenIds);

	m_files = getFilesForActiveSourceLocations(m_collection.get(), 0);
	createReferences();
	expandVisibleFiles(m_codeParams.useSingleFileCache);
	showFiles(m_codeParams, firstReferenceScrollParams(), !message->isReplayed());
}

void CodeController::handleMessage(MessageChangeFileView* message)
{
	TRACE("code change file");

	saveOrRestoreViewMode(message);

	for (CodeFileParams& file: m_files)
	{
		if (file.locationFile->getFilePath() == message->filePath)
		{
			setFileState(file, message->state, m_codeParams.useSingleFileCache);

			// maximize within snippet list
			if (message->viewMode == MessageChangeFileView::VIEW_LIST &&
				message->state == MessageChangeFileView::FILE_MAXIMIZED)
			{
				file.snippetParams = {*file.fileParams.get()};
			}

			break;
		}
	}

	showFiles(m_codeParams, message->scrollParams, !message->isReplayed());
}

void CodeController::handleMessage(MessageCodeReference* message)
{
	bool next = (message->type == MessageCodeReference::REFERENCE_NEXT);
	bool local = message->localReference;

	if (local)
	{
		iterateLocalReference(next, true);
	}
	else
	{
		iterateReference(next);
	}

	MessageFocusView(MessageFocusView::ViewType::CODE).dispatch();
}

void CodeController::handleMessage(MessageCodeShowDefinition* message)
{
	TRACE("code show definition");

	Id nodeId = message->nodeId;

	std::shared_ptr<SourceLocationCollection> collection =
		m_storageAccess->getSourceLocationsForTokenIds({nodeId});
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
		std::shared_ptr<SourceLocationCollection> filteredCollection =
			std::make_shared<SourceLocationCollection>();
		bool addedLocation = false;

		collection->forEachSourceLocation([&](SourceLocation* location) {
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
		});

		if (!addedLocation)
		{
			SourceLocation* location =
				collection->getSourceLocationFiles().begin()->second->getSourceLocations().begin()->get();
			filteredCollection->addSourceLocationCopy(location);
			filteredCollection->addSourceLocationCopy(location->getOtherLocation());

			filePath = location->getFilePath();
			lineNumber = location->getStartLocation()->getLineNumber();
			columnNumber = location->getStartLocation()->getColumnNumber();
		}

		collection = filteredCollection;
	}
	else	// otherwise first file
	{
		filePath = collection->getSourceLocationFiles().begin()->second->getFilePath();
	}

	if (message->inIDE)
	{
		MessageMoveIDECursor(
			filePath, static_cast<unsigned int>(lineNumber), static_cast<unsigned int>(columnNumber))
			.dispatch();
		return;
	}

	CodeFileParams* file = addSourceLocations(collection->getSourceLocationFiles().begin()->second);
	if (!file)
	{
		LOG_ERROR("MessageCodeShowDefinition did not create a file");
		return;
	}

	saveOrRestoreViewMode(message);

	showFiles(
		m_codeParams,
		CodeScrollParams::toLine(filePath, lineNumber, CodeScrollParams::Target::TOP),
		!message->isReplayed());
}

void CodeController::handleMessage(MessageDeactivateEdge* message)
{
	CodeScrollParams scrollParams;
	m_codeParams.activeLocationIds.clear();
	m_codeParams.activeLocalSymbolIds.clear();
	m_codeParams.currentActiveLocalLocationIds.clear();
	m_referenceIndex = -1;
	clearLocalReferences();

	if (message->scrollToDefinition)
	{
		scrollParams = definitionReferenceScrollParams(m_codeParams.activeTokenIds);
	}

	showFiles(m_codeParams, scrollParams, !message->isReplayed());
}

void CodeController::handleMessage(MessageErrorCountClear* message)
{
	if (getView()->showsErrors())
	{
		clear();
	}
}

void CodeController::handleMessage(MessageFocusChanged* message)
{
	if (message->isReplayed() && message->isFromCode())
	{
		m_codeParams.locationIdToFocus = message->tokenOrLocationId;
	}
}

void CodeController::handleMessage(MessageFlushUpdates* message)
{
	showFiles(m_codeParams, m_scrollParams, true);
}

void CodeController::handleMessage(MessageFocusIn* message)
{
	getView()->coFocusTokenIds(message->tokenIds);
}

void CodeController::handleMessage(MessageFocusOut* message)
{
	getView()->deCoFocusTokenIds();
}

void CodeController::handleMessage(MessageScrollToLine* message)
{
	getView()->scrollTo(
		CodeScrollParams::toLine(message->filePath, message->line, CodeScrollParams::Target::TOP),
		false);

	MessageStatus(
		L"Showing source location: " + message->filePath.wstr() + L" : " +
		std::to_wstring(message->line))
		.dispatch();
}

void CodeController::handleMessage(MessageScrollCode* message)
{
	if (message->isReplayed())
	{
		m_scrollParams = CodeScrollParams::toValue(message->value, message->inListMode);
	}
}

void CodeController::handleMessage(MessageShowError* message)
{
	CodeView* view = getView();
	if (view->showsErrors())
	{
		showFirstActiveReference(message->errorId, !message->isReplayed());
	}
}

void CodeController::handleMessage(MessageShowReference* message)
{
	m_referenceIndex = static_cast<int>(message->refIndex);
	bool replayed = message->isReplayed();

	if (m_referenceIndex >= 0 && m_referenceIndex < static_cast<int>(m_references.size()))
	{
		const Reference& ref = m_references[m_referenceIndex];
		m_codeParams.activeLocationIds = {ref.locationId};

		setFileState(
			ref.filePath,
			getView()->isInListMode() ? MessageChangeFileView::FILE_SNIPPETS
									  : MessageChangeFileView::FILE_MAXIMIZED,
			m_codeParams.useSingleFileCache);

		showFiles(m_codeParams, toReferenceScrollParams(ref), !message->isReplayed());

		if (ref.locationType == LOCATION_ERROR)
		{
			MessageShowError(ref.tokenId).dispatch();
		}
	}
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

	addSourceLocations(collection->getSourceLocationFiles().begin()->second);

	if (m_localReferences.size())
	{
		addAllSourceLocations();
		createLocalReferences(utility::toSet(m_codeParams.activeLocalSymbolIds));
		m_codeParams.currentActiveLocalLocationIds.clear();
	}

	showFiles(m_codeParams, CodeScrollParams(), !message->isReplayed());
}

void CodeController::handleMessage(MessageToNextCodeReference* message)
{
	FilePath currentFilePath = message->filePath;
	size_t currentLineNumber = message->lineNumber;
	size_t currentColumnNumber = message->columnNumber;
	bool next = message->next;
	bool inListMode = getView()->isInListMode();

	if (currentFilePath.empty())
	{
		return;
	}

	std::pair<int, int> referencePos = findClosestReferenceIndex(
		m_references, currentFilePath, currentLineNumber, currentColumnNumber, next);
	std::pair<int, int> localReferencePos = findClosestReferenceIndex(
		m_localReferences, currentFilePath, currentLineNumber, currentColumnNumber, next);

	int referenceIndex = referencePos.first;
	int referenceFileIndex = referencePos.second;
	int localReferenceIndex = localReferencePos.first;
	int localReferenceFileIndex = localReferencePos.second;

	if (referenceIndex >= 0 && localReferenceIndex >= 0)
	{
		if (referenceFileIndex == localReferenceFileIndex)
		{
			if (referenceFileIndex < 0)
			{
				if (m_references[referenceIndex].filePath !=
						m_localReferences[localReferenceIndex].filePath ||
					m_references[referenceIndex].lineNumber >
						m_localReferences[localReferenceIndex].lineNumber)
				{
					localReferenceIndex = -1;
				}
				else
				{
					referenceIndex = -1;
				}
			}
			else if (referenceFileIndex == 0)
			{
				if (m_references[referenceIndex].lineNumber ==
					m_localReferences[localReferenceIndex].lineNumber)
				{
					if ((next &&
						 m_references[referenceIndex].columnNumber <
							 m_localReferences[localReferenceIndex].columnNumber) ||
						(!next &&
						 m_references[referenceIndex].columnNumber >
							 m_localReferences[localReferenceIndex].columnNumber))
					{
						localReferenceIndex = -1;
					}
					else
					{
						referenceIndex = -1;
					}
				}
				else
				{
					if ((next &&
						 m_references[referenceIndex].lineNumber <
							 m_localReferences[localReferenceIndex].lineNumber) ||
						(!next &&
						 m_references[referenceIndex].lineNumber >
							 m_localReferences[localReferenceIndex].lineNumber))
					{
						localReferenceIndex = -1;
					}
					else
					{
						referenceIndex = -1;
					}
				}
			}
			else
			{
				if (m_references[referenceIndex].filePath !=
						m_localReferences[localReferenceIndex].filePath ||
					m_references[referenceIndex].lineNumber <
						m_localReferences[localReferenceIndex].lineNumber)
				{
					localReferenceIndex = -1;
				}
				else
				{
					referenceIndex = -1;
				}
			}
		}
		else if (referenceFileIndex == 0)
		{
			localReferenceIndex = -1;
		}
		else if (localReferenceFileIndex == 0)
		{
			referenceIndex = -1;
		}
	}

	if (localReferenceIndex >= 0)
	{
		m_localReferenceIndex = localReferenceIndex;
		showCurrentLocalReference(true);
	}
	else if (referenceIndex >= 0)
	{
		m_referenceIndex = referenceIndex;
		showCurrentReference();
	}
}

CodeView* CodeController::getView() const
{
	return Controller::getView<CodeView>();
}

void CodeController::clear()
{
	getView()->clear();

	m_collection = std::make_shared<SourceLocationCollection>();
	m_currentFilePath = FilePath();
	clearReferences();
}

std::vector<CodeFileParams> CodeController::getFilesForActiveSourceLocations(
	const SourceLocationCollection* collection, Id declarationId) const
{
	TRACE();

	std::vector<CodeFileParams> files;
	collection->forEachSourceLocationFile([&](std::shared_ptr<SourceLocationFile> file) -> void {
		bool isDeclarationFile = false;
		bool isDefinitionFile = false;
		file->forEachSourceLocation([&](SourceLocation* location) {
			for (Id i: location->getTokenIds())
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
		});

		CodeFileParams params;
		params.locationFile = file;
		params.isDeclaration = isDeclarationFile;
		params.isDefinition = isDefinitionFile;
		files.push_back(params);
	});

	std::sort(files.begin(), files.end(), CodeFileParams::sort);

	return files;
}

std::vector<CodeFileParams> CodeController::getFilesForCollection(
	std::shared_ptr<SourceLocationCollection> collection) const
{
	TRACE();

	std::vector<CodeFileParams> files;

	collection->forEachSourceLocationFile([&](std::shared_ptr<SourceLocationFile> file) -> void {
		CodeFileParams params;
		params.locationFile = file;
		files.push_back(params);
	});

	return files;
}


CodeSnippetParams CodeController::getSnippetParamsForWholeFile(
	std::shared_ptr<SourceLocationFile> locationFile, bool useSingleFileCache) const
{
	CodeSnippetParams snippet;
	snippet.startLineNumber = 1;

	if (useSingleFileCache && !getView()->isInListMode() &&
		getView()->hasSingleFileCached(locationFile->getFilePath()))
	{
		snippet.locationFile = locationFile;
		return snippet;
	}

	bool showsErrors = false;
	if (m_collection->getSourceLocationFiles().size())
	{
		std::shared_ptr<SourceLocationFile> file =
			m_collection->getSourceLocationFiles().begin()->second;
		if (file->getSourceLocations().size())
		{
			showsErrors = (*file->getSourceLocations().begin())->getType() == LOCATION_ERROR;
		}
	}

	std::shared_ptr<TextAccess> textAccess = m_storageAccess->getFileContent(
		locationFile->getFilePath(), showsErrors);
	snippet.code = textAccess->getText();

	// make a copy of SourceLocationFile so that isWhole flag is different for first snippet adding
	// the file and second snippet adding the content
	snippet.locationFile = std::make_shared<SourceLocationFile>(*locationFile.get());
	snippet.locationFile->setIsWhole(true);

	return snippet;
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForFile(
	std::shared_ptr<SourceLocationFile> activeSourceLocations) const
{
	TRACE();

	bool showsErrors = false;
	if (activeSourceLocations->getSourceLocations().size())
	{
		showsErrors = (*activeSourceLocations->getSourceLocations().begin())->getType() ==
			LOCATION_ERROR;
	}

	std::shared_ptr<TextAccess> textAccess = m_storageAccess->getFileContent(
		activeSourceLocations->getFilePath(), showsErrors);
	size_t lineCount = textAccess->getLineCount();

	SnippetMerger fileScopedMerger(1, static_cast<int>(lineCount));
	std::map<int, std::shared_ptr<SnippetMerger>> mergers;

	std::shared_ptr<SourceLocationFile> scopeLocations =
		m_storageAccess->getSourceLocationsOfTypeInFile(
			activeSourceLocations->getFilePath(), LOCATION_SCOPE);
	activeSourceLocations->forEachStartSourceLocation([&](SourceLocation* location) {
		buildMergerHierarchy(location, scopeLocations.get(), fileScopedMerger, mergers);
	});

	std::vector<SnippetMerger::Range> atomicRanges;
	std::shared_ptr<SourceLocationFile> commentLocations =
		m_storageAccess->getSourceLocationsOfTypeInFile(
			activeSourceLocations->getFilePath(), LOCATION_COMMENT);
	commentLocations->forEachStartSourceLocation([&](SourceLocation* location) {
		atomicRanges.push_back(SnippetMerger::Range(
			SnippetMerger::Border(static_cast<int>(location->getLineNumber()), false),
			SnippetMerger::Border(
				static_cast<int>(location->getOtherLocation()->getLineNumber()), false)));
	});

	atomicRanges = SnippetMerger::Range::mergeAdjacent(atomicRanges);
	std::deque<SnippetMerger::Range> ranges = fileScopedMerger.merge(atomicRanges);

	const int snippetExpandRange = ApplicationSettings::getInstance()->getCodeSnippetExpandRange();
	std::vector<CodeSnippetParams> snippets;

	for (const SnippetMerger::Range& range: ranges)
	{
		CodeSnippetParams params;
		params.startLineNumber = std::max<int>(
			1, range.start.row - (range.start.strong ? 0 : snippetExpandRange));
		params.endLineNumber = std::min<int>(
			static_cast<int>(lineCount), range.end.row + (range.end.strong ? 0 : snippetExpandRange));

		params.locationFile = activeSourceLocations->getFilteredByLines(
			params.startLineNumber, params.endLineNumber);

		if (params.startLineNumber > 1)
		{
			const SourceLocation* location = getSourceLocationOfParentScope(
				params.startLineNumber, scopeLocations.get());
			if (location && location->getTokenIds().size())
			{
				params.title = m_storageAccess->getNameHierarchyForNodeId(location->getTokenIds()[0])
								   .getQualifiedName();
				params.titleId = location->getLocationId();
			}
		}

		if (params.endLineNumber < lineCount)
		{
			const SourceLocation* location = getSourceLocationOfParentScope(
				params.endLineNumber + 1, scopeLocations.get());
			if (location && location->getTokenIds().size())
			{
				params.footer = m_storageAccess
									->getNameHierarchyForNodeId(location->getTokenIds()[0])
									.getQualifiedName();
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

		for (const std::string& line: textAccess->getLines(
				 static_cast<unsigned int>(params.startLineNumber),
				 static_cast<unsigned int>(params.endLineNumber)))
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
	std::map<int, std::shared_ptr<SnippetMerger>>& mergers) const
{
	std::shared_ptr<SnippetMerger> currentMerger = std::make_shared<SnippetMerger>(
		static_cast<int>(location->getStartLocation()->getLineNumber()),
		static_cast<int>(location->getEndLocation()->getLineNumber()));

	const SourceLocation* scopeLocation = getSourceLocationOfParentScope(
		location->getLineNumber(), scopeLocations);
	if (!scopeLocation)
	{
		fileScopedMerger.addChild(currentMerger);
		return currentMerger;
	}

	std::shared_ptr<SnippetMerger> nextMerger;
	std::map<int, std::shared_ptr<SnippetMerger>>::iterator it = mergers.find(
		static_cast<int>(scopeLocation->getLocationId()));
	if (it == mergers.end())
	{
		nextMerger = buildMergerHierarchy(scopeLocation, scopeLocations, fileScopedMerger, mergers);
		mergers[static_cast<int>(scopeLocation->getLocationId())] = nextMerger;
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

	scopeLocations->forEachStartSourceLocation([&location, lineNumber](SourceLocation* scopeLocation) {
		if (scopeLocation->getLineNumber() < lineNumber &&
			scopeLocation->getEndLocation()->getLineNumber() >= lineNumber &&
			(!location || *location < *scopeLocation))
		{
			location = scopeLocation;
		}
	});

	return location;
}

std::vector<std::string> CodeController::getProjectDescription(SourceLocationFile* locationFile) const
{
	std::shared_ptr<const Project> currentProject = Application::getInstance()->getCurrentProject();
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

	Id locationId = 0;
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

			std::wstring serializedName = utility::decodeFromUtf8(
				line.substr(posA + 1, posB - posA - 1));

			NameHierarchy nameHierarchy = NameHierarchy::deserialize(serializedName);
			Id tokenId = m_storageAccess->getNodeIdForNameHierarchy(nameHierarchy);

			std::string nameString = utility::encodeToUtf8(nameHierarchy.getQualifiedName());
			if (tokenId > 0)
			{
				line.replace(posA, posB - posA + 1, nameString);
				locationFile->addSourceLocation(
					LOCATION_TOKEN,
					++locationId,
					{tokenId},
					startLineNumber + i,
					posA + 1,
					startLineNumber + i,
					posA + nameString.size());
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

void CodeController::clearReferences()
{
	m_references.clear();
	m_referenceIndex = -1;

	clearLocalReferences();
}

void CodeController::createReferences()
{
	clearReferences();

	for (CodeFileParams& file: m_files)
	{
		size_t referenceCountBefore = m_references.size();

		if (file.locationFile->isWhole())
		{
			Reference ref;
			ref.filePath = file.locationFile->getFilePath();
			m_references.push_back(ref);
		}
		else
		{
			std::map<Id, Id> scopeLocationIds;

			file.locationFile->forEachStartSourceLocation([&](SourceLocation* location) {
				if (location->isScopeLocation())
				{
					for (Id tokenId: location->getTokenIds())
					{
						scopeLocationIds.emplace(tokenId, location->getLocationId());
					}
				}
			});

			file.locationFile->forEachStartSourceLocation([&](SourceLocation* location) {
				if (location->isScopeLocation() || location->getType() == LOCATION_SIGNATURE ||
					location->getType() == LOCATION_COMMENT ||
					location->getType() == LOCATION_QUALIFIER)
				{
					return;
				}

				if (!location->getTokenIds().size())
				{
					Reference ref;
					ref.filePath = location->getFilePath();
					ref.tokenId = 0;
					ref.locationId = location->getLocationId();
					ref.locationType = location->getType();
					ref.lineNumber = location->getLineNumber();
					ref.columnNumber = location->getColumnNumber();
					m_references.push_back(ref);
					return;
				}

				for (Id i: location->getTokenIds())
				{
					Reference ref;
					ref.filePath = location->getFilePath();
					ref.tokenId = i;
					ref.locationId = location->getLocationId();
					ref.locationType = location->getType();
					ref.lineNumber = location->getLineNumber();
					ref.columnNumber = location->getColumnNumber();

					std::map<Id, Id>::const_iterator it = scopeLocationIds.find(i);
					if (it != scopeLocationIds.end())
					{
						ref.scopeLocationId = it->second;
					}

					m_references.push_back(ref);
				}
			});
		}

		file.referenceCount = m_references.size() - referenceCountBefore;
	}
}

void CodeController::clearLocalReferences()
{
	m_localReferences.clear();
	m_localReferenceIndex = -1;
}

void CodeController::createLocalReferences(const std::set<Id>& localSymbolIds)
{
	clearLocalReferences();

	if (localSymbolIds.size())
	{
		auto func = [&localSymbolIds, this](const SourceLocation* location) {
			for (Id tokenId: location->getTokenIds())
			{
				if (localSymbolIds.find(tokenId) != localSymbolIds.end())
				{
					Reference ref;
					ref.filePath = location->getFilePath();
					ref.locationId = location->getLocationId();
					ref.locationType = location->getType();
					ref.lineNumber = location->getLineNumber();
					ref.columnNumber = location->getColumnNumber();
					m_localReferences.push_back(ref);
					return;
				}
			}
		};

		bool inListMode = getView()->isInListMode();
		for (const CodeFileParams& file: m_files)
		{
			if (inListMode)
			{
				for (const CodeSnippetParams& snippet: file.snippetParams)
				{
					snippet.locationFile->forEachStartSourceLocation(func);
				}
			}
			else if (file.fileParams)
			{
				file.fileParams->locationFile->forEachStartSourceLocation(func);
			}
		}
	}
}

void CodeController::iterateReference(bool next)
{
	if (!m_references.size())
	{
		return;
	}

	if (next)
	{
		m_referenceIndex++;

		if (m_referenceIndex == m_references.size())
		{
			m_referenceIndex = 0;
		}
	}
	else
	{
		if (m_referenceIndex < 1)
		{
			m_referenceIndex = static_cast<int>(m_references.size()) - 1;
		}
		else
		{
			m_referenceIndex--;
		}
	}

	showCurrentReference();
}

void CodeController::iterateLocalReference(bool next, bool updateView)
{
	if (!m_localReferences.size())
	{
		return;
	}

	if (next)
	{
		m_localReferenceIndex++;

		if (m_localReferenceIndex == static_cast<int>(m_localReferences.size()))
		{
			m_localReferenceIndex = 0;
		}
	}
	else
	{
		if (m_localReferenceIndex < 1)
		{
			m_localReferenceIndex = static_cast<int>(m_localReferences.size()) - 1;
		}
		else
		{
			m_localReferenceIndex--;
		}
	}

	showCurrentLocalReference(updateView);
}

void CodeController::showCurrentReference()
{
	const Reference& ref = m_references[m_referenceIndex];
	MessageShowReference(m_referenceIndex, ref.tokenId, ref.locationId, true).dispatch();
}

void CodeController::showCurrentLocalReference(bool updateView)
{
	const Reference& ref = m_localReferences[m_localReferenceIndex];
	m_codeParams.currentActiveLocalLocationIds = {ref.locationId};

	// synchronise reference navigation with local reference navigation
	if (ref.locationType == LOCATION_TOKEN)
	{
		for (size_t i = 0; i < m_references.size(); i++)
		{
			if (m_references[i].locationId == ref.locationId)
			{
				m_referenceIndex = static_cast<int>(i);
			}
		}
	}

	showFiles(m_codeParams, toReferenceScrollParams(ref), updateView);
}

std::pair<int, int> CodeController::findClosestReferenceIndex(
	const std::vector<Reference>& references,
	const FilePath& currentFilePath,
	size_t currentLineNumber,
	size_t currentColumnNumber,
	bool next) const
{
	int referenceIndex = -1;
	bool beforeCurrentFile = true;

	for (size_t i = 0; i < references.size(); i++)
	{
		if (references[i].filePath == currentFilePath)
		{
			if (!next)
			{
				if (references[i].lineNumber < currentLineNumber ||
					(references[i].lineNumber == currentLineNumber &&
					 references[i].columnNumber < currentColumnNumber))
				{
					referenceIndex = static_cast<int>(i);
				}
				else
				{
					return {referenceIndex, beforeCurrentFile ? -1 : 0};
				}
			}
			else if (
				references[i].lineNumber > currentLineNumber ||
				(references[i].lineNumber == currentLineNumber &&
				 references[i].columnNumber > currentColumnNumber))
			{
				return {static_cast<int>(i), 0};
			}

			beforeCurrentFile = false;
		}
		else if (!next)
		{
			if (beforeCurrentFile)
			{
				referenceIndex = static_cast<int>(i);
			}
			else
			{
				return {referenceIndex, 1};
			}
		}
		else if (next && !beforeCurrentFile)
		{
			return {static_cast<int>(i), 1};
		}
	}

	int fileIndex = beforeCurrentFile ? -1 : 1;
	if (referenceIndex >= 0 && references[referenceIndex].filePath == currentFilePath)
	{
		fileIndex = 0;
	}

	return {referenceIndex, fileIndex};
}

void CodeController::expandVisibleFiles(bool useSingleFileCache)
{
	TRACE();

	if (!m_files.size())
	{
		return;
	}

	bool inListMode = getView()->isInListMode();
	size_t filesToExpand = inListMode ? std::min(int(m_files.size()), 3) : 1;
	MessageChangeFileView::FileState state = inListMode ? MessageChangeFileView::FILE_SNIPPETS
														: MessageChangeFileView::FILE_MAXIMIZED;

	for (size_t i = 0; i < filesToExpand; i++)
	{
		setFileState(m_files[i], state, useSingleFileCache);
	}
}

CodeFileParams* CodeController::addSourceLocations(std::shared_ptr<SourceLocationFile> locationFile)
{
	if (!m_collection)
	{
		m_collection = std::make_shared<SourceLocationCollection>();
	}
	m_collection->addSourceLocationCopies(locationFile.get());

	CodeFileParams* file = nullptr;
	for (CodeFileParams& f: m_files)
	{
		if (f.locationFile->getFilePath() == locationFile->getFilePath())
		{
			file = &f;
			break;
		}
	}

	if (!file)
	{
		CodeFileParams f;
		f.locationFile = locationFile;
		m_files.push_back(f);
		file = &m_files.back();
	}

	if (file->snippetParams.size())
	{
		std::vector<CodeSnippetParams> snippets = getSnippetsForFile(locationFile);
		if (snippets.size() != 1)
		{
			LOG_ERROR("addSourceLocations() didn't result in one single snippet to be created");
			return nullptr;
		}

		CodeSnippetParams newSnippet = snippets[0];

		size_t i = 0;
		while (i < file->snippetParams.size())
		{
			size_t start = newSnippet.startLineNumber;
			size_t end = newSnippet.endLineNumber;

			const CodeSnippetParams& oldSnippet = file->snippetParams[i];

			if (oldSnippet.endLineNumber + 1 < start)	 // before
			{
				i++;
				continue;
			}
			else if (oldSnippet.startLineNumber > end + 1)	  // after
			{
				break;
			}
			else if (oldSnippet.startLineNumber <= start && oldSnippet.endLineNumber >= end)	// containing
			{
				return nullptr;
			}
			else if (oldSnippet.startLineNumber < start || oldSnippet.endLineNumber > end)	  // overlaping
			{
				newSnippet = CodeSnippetParams::merge(newSnippet, oldSnippet);
			}
			else if (oldSnippet.startLineNumber >= start && oldSnippet.endLineNumber <= end)	// enclosing
			{
				// copy all source locations from old to new snippet: fulltext locations got lost
				newSnippet.locationFile->copySourceLocations(oldSnippet.locationFile);
			}

			file->snippetParams.erase(file->snippetParams.begin() + i);
		}

		file->snippetParams.insert(file->snippetParams.begin() + i, newSnippet);
	}

	setFileState(
		*file,
		getView()->isInListMode() ? MessageChangeFileView::FILE_SNIPPETS
								  : MessageChangeFileView::FILE_MAXIMIZED,
		m_codeParams.useSingleFileCache);
	return file;
}

void CodeController::setFileState(
	const FilePath& filePath, MessageChangeFileView::FileState state, bool useSingleFileCache)
{
	for (CodeFileParams& file: m_files)
	{
		if (file.locationFile->getFilePath() == filePath)
		{
			setFileState(file, state, useSingleFileCache);
			return;
		}
	}
}

void CodeController::setFileState(
	CodeFileParams& file, MessageChangeFileView::FileState state, bool useSingleFileCache)
{
	TRACE();

	switch (state)
	{
	case MessageChangeFileView::FILE_MINIMIZED:
		file.isMinimized = true;
		break;

	case MessageChangeFileView::FILE_SNIPPETS:
		file.isMinimized = false;
		if (!file.snippetParams.size())
		{
			if (file.locationFile->isWhole())
			{
				file.snippetParams = {
					getSnippetParamsForWholeFile(file.locationFile, useSingleFileCache)};
			}
			else
			{
				file.snippetParams = getSnippetsForFile(file.locationFile);
			}
		}
		break;

	case MessageChangeFileView::FILE_MAXIMIZED:
		if (!file.fileParams)
		{
			file.fileParams = std::make_shared<CodeSnippetParams>(
				getSnippetParamsForWholeFile(file.locationFile, useSingleFileCache));
		}

		if (file.locationFile)
		{
			m_currentFilePath = file.locationFile->getFilePath();
		}
		break;
	}
}

bool CodeController::addAllSourceLocations()
{
	TRACE();

	bool addedNewLocations = false;

	for (CodeFileParams& file: m_files)
	{
		for (CodeSnippetParams& snippet: file.snippetParams)
		{
			if (snippet.hasAllSourceLocations)
			{
				continue;
			}

			if (snippet.locationFile->isWhole())
			{
				snippet.locationFile = m_storageAccess->getSourceLocationsForFile(
					snippet.locationFile->getFilePath());
				if (snippet.locationFile)
				{
					snippet.locationFile->copySourceLocations(file.locationFile);
				}
			}
			else
			{
				std::shared_ptr<SourceLocationFile> file =
					m_storageAccess->getSourceLocationsForLinesInFile(
						snippet.locationFile->getFilePath(),
						snippet.startLineNumber,
						snippet.endLineNumber);
				if (file)
				{
					file->copySourceLocations(snippet.locationFile);
					snippet.locationFile = file;
				}
			}

			addedNewLocations = true;
			snippet.hasAllSourceLocations = true;
		}

		if (file.fileParams)
		{
			if (file.fileParams->hasAllSourceLocations)
			{
				continue;
			}

			file.fileParams->locationFile = m_storageAccess->getSourceLocationsForFile(
				file.locationFile->getFilePath());
			if (file.fileParams->locationFile)
			{
				file.fileParams->locationFile->copySourceLocations(file.locationFile);
			}

			addedNewLocations = true;
			file.fileParams->hasAllSourceLocations = true;
		}
	}

	return addedNewLocations;
}

void CodeController::addModificationTimes()
{
	TRACE();

	std::vector<FilePath> filePaths;
	for (const CodeFileParams& file: m_files)
	{
		filePaths.push_back(file.locationFile->getFilePath());
	}
	std::vector<FileInfo> fileInfos = m_storageAccess->getFileInfosForFilePaths(filePaths);

	std::map<FilePath, FileInfo> fileInfoMap;
	for (FileInfo& fileInfo: fileInfos)
	{
		fileInfoMap.emplace(fileInfo.path, fileInfo);
	}

	for (CodeFileParams& file: m_files)
	{
		file.modificationTime = fileInfoMap[file.locationFile->getFilePath()].lastWriteTime;
	}
}

CodeScrollParams CodeController::firstReferenceScrollParams() const
{
	if (m_references.size())
	{
		const Reference& ref = m_references.front();

		return CodeScrollParams::toReference(
			ref.filePath, ref.locationId, ref.scopeLocationId, CodeScrollParams::Target::TOP);
	}

	return CodeScrollParams();
}

CodeScrollParams CodeController::definitionReferenceScrollParams(const std::vector<Id>& activeTokenIds) const
{
	Id activeTokenId = activeTokenIds.size() ? activeTokenIds.front() : 0;
	if (activeTokenId)
	{
		for (const Reference& ref: m_references)
		{
			if (ref.scopeLocationId && ref.tokenId == activeTokenId)
			{
				return CodeScrollParams::toReference(
					ref.filePath, ref.locationId, ref.scopeLocationId, CodeScrollParams::Target::TOP);
			}
		}

		for (const Reference& ref: m_references)
		{
			if (ref.tokenId == activeTokenId)
			{
				return CodeScrollParams::toReference(
					ref.filePath, ref.locationId, ref.scopeLocationId, CodeScrollParams::Target::TOP);
			}
		}
	}

	return firstReferenceScrollParams();
}

CodeScrollParams CodeController::toReferenceScrollParams(const Reference& ref) const
{
	return CodeScrollParams::toReference(
		ref.filePath, ref.locationId, ref.scopeLocationId, CodeScrollParams::Target::CENTER);
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

void CodeController::showFirstActiveReference(Id tokenId, bool updateView)
{
	// iterate local references when same tokenId get reactivated (consecutive edge clicks)
	if (m_codeParams.activeLocalSymbolIds.size() == 1 &&
		m_codeParams.activeLocalSymbolIds[0] == tokenId && m_localReferences.size())
	{
		iterateLocalReference(true, updateView);
		return;
	}

	std::vector<Id> locationIds;
	int referenceIndex = -1;
	Reference firstReference;

	std::set<FilePath> filePathsToExpand;
	std::map<FilePath, size_t> filePathOrder;

	for (size_t i = 0; i < m_references.size(); i++)
	{
		const Reference& ref = m_references[i];
		if (ref.tokenId == tokenId)
		{
			filePathsToExpand.insert(ref.filePath);
			locationIds.push_back(ref.locationId);

			if (!firstReference.tokenId)
			{
				firstReference = ref;
				referenceIndex = static_cast<int>(i);
			}
		}

		filePathOrder.emplace(ref.filePath, filePathOrder.size());
	}

	if (!locationIds.size())
	{
		std::shared_ptr<SourceLocationCollection> collection =
			m_storageAccess->getSourceLocationsForTokenIds({tokenId});
		collection->forEachSourceLocation([&](SourceLocation* location) {
			if (!location->isStartLocation())
			{
				return;
			}

			for (Id i: location->getTokenIds())
			{
				if (i == tokenId)
				{
					locationIds.push_back(location->getLocationId());
					filePathsToExpand.insert(location->getFilePath());

					if (!firstReference.tokenId ||
						filePathOrder[location->getFilePath()] <
							filePathOrder[firstReference.filePath])
					{
						firstReference.tokenId = tokenId;
						firstReference.locationId = location->getLocationId();
						firstReference.filePath = location->getFilePath();
					}
					return;
				}
			}
		});
	}

	if (getView()->isInListMode())
	{
		for (const FilePath& filePath: filePathsToExpand)
		{
			setFileState(
				filePath, MessageChangeFileView::FILE_SNIPPETS, m_codeParams.useSingleFileCache);
		}
	}
	else if (firstReference.tokenId)
	{
		setFileState(
			firstReference.filePath,
			MessageChangeFileView::FILE_MAXIMIZED,
			m_codeParams.useSingleFileCache);
	}

	createLocalReferences({tokenId});
	m_codeParams.activeLocalSymbolIds = {tokenId};
	m_codeParams.activeLocalSymbolType = LOCATION_TOKEN;
	m_codeParams.currentActiveLocalLocationIds.clear();

	CodeScrollParams scrollParams;

	if (firstReference.tokenId)
	{
		scrollParams = toReferenceScrollParams(firstReference);
		m_referenceIndex = referenceIndex;
	}

	showFiles(m_codeParams, scrollParams, updateView);
}

void CodeController::showFiles(CodeView::CodeParams params, CodeScrollParams scrollParams, bool updateView)
{
	if (updateView)
	{
		addModificationTimes();

		params.referenceCount = m_references.size();
		params.referenceIndex = m_referenceIndex >= 0 ? m_referenceIndex : m_references.size();

		params.localReferenceCount = m_localReferences.size();
		params.localReferenceIndex = m_localReferenceIndex >= 0 ? m_localReferenceIndex
																: m_localReferences.size();

		if (getView()->isInListMode())
		{
			getView()->showSnippets(m_files, params, scrollParams);
		}
		else
		{
			bool updated = false;
			for (const CodeFileParams& file: m_files)
			{
				if (file.locationFile->getFilePath() == m_currentFilePath)
				{
					getView()->showSingleFile(file, params, scrollParams);
					updated = true;
					break;
				}
			}

			if (!updated)
			{
				getView()->showSingleFile(CodeFileParams(), params, scrollParams);
			}
		}

		params.clearSnippets = false;
		m_scrollParams = CodeScrollParams();
	}
	else if (scrollParams.type != CodeScrollParams::Type::NONE)
	{
		m_scrollParams = scrollParams;
	}

	m_codeParams = params;

	if (addAllSourceLocations() && updateView)
	{
		getView()->updateSourceLocations(m_files);
	}
}
