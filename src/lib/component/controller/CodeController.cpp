#include "component/controller/CodeController.h"

#include <memory>

#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/tracing.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "data/access/StorageAccess.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "settings/ApplicationSettings.h"
#include "Application.h"

CodeController::CodeController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
	, m_scrollToDefinition(false)
	, m_scrollToValue(-1)
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

	StorageStats stats = m_storageAccess->getStorageStats();
	ErrorCountInfo errorCount = m_storageAccess->getErrorCount();
	CodeSnippetParams statsSnippet;

	statsSnippet.startLineNumber = 1;
	statsSnippet.endLineNumber = 1;

	statsSnippet.reduced = true;

	statsSnippet.locationFile = std::make_shared<TokenLocationFile>(FilePath());
	statsSnippet.locationFile->isWholeCopy = true;

	Project* currentProject = Application::getInstance()->getCurrentProject().get();
	if (currentProject)
	{
		statsSnippet.title = currentProject->getProjectSettingsFilePath().withoutExtension().fileName();
	}

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

	ss << "\n";
	ss << "\t" + std::to_string(stats.fileCount) + " files\n";
	ss << "\t" + std::to_string(stats.fileLOCCount) + " lines of code\n";
	ss << "\n";
	ss << "\t" + std::to_string(stats.nodeCount) + " symbols\n";
	ss << "\t" + std::to_string(stats.edgeCount) + " relations\n";
	ss << "\n";
	ss << "\t" + std::to_string(errorCount.total) + " errors (" + std::to_string(errorCount.fatal) + " fatal)\n";
	ss << "\n";

	if (errorCount.total > 0)
	{
		ss << "\tWarning: Indexing may be incomplete as long as it yields fatal errors.\n";
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

	if (!message->keepContent())
	{
		view->clear();
	}

	std::vector<Id> activeTokenIds = message->tokenIds;
	if (!activeTokenIds.size())
	{
		return;
	}

	Id declarationId = 0; // 0 means that no token is found.
	if (!message->isAggregation)
	{
		activeTokenIds = m_storageAccess->getActiveTokenIdsForId(activeTokenIds[0], &declarationId);
	}

	if (message->isEdge)
	{
		std::shared_ptr<TokenLocationCollection> collection = m_storageAccess->getTokenLocationsForTokenIds(activeTokenIds);
		view->showActiveSnippet(activeTokenIds, collection, message->isLast());
	}
	else if (message->keepContent())
	{
		view->showActiveTokenIds(activeTokenIds);
	}
	else
	{
		m_collection = m_storageAccess->getTokenLocationsForTokenIds(activeTokenIds);
		view->showCodeSnippets(getSnippetsForActiveTokenLocations(m_collection.get(), declarationId), activeTokenIds, true);
		m_scrollToDefinition = !message->isReplayed() || message->isLast();

		size_t fileCount = m_collection->getTokenLocationFileCount();
		size_t referenceCount = m_collection->getTokenLocationCount();

		std::stringstream ss;

		if (message->unknownNames.size())
		{
			ss << "Activate \"" << message->unknownNames[0] << "\": ";
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

	switch (message->state)
	{
	case MessageChangeFileView::FILE_MINIMIZED:
		view->setFileState(message->filePath, CodeView::FILE_MINIMIZED);
		break;

	case MessageChangeFileView::FILE_SNIPPETS:
		if (message->needsData)
		{
			std::shared_ptr<TokenLocationFile> file = m_collection->getTokenLocationFileByPath(message->filePath);
			if (!file)
			{
				return;
			}

			if (message->showErrors)
			{
				file->isWholeCopy = false;
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
				params.locationFile = m_collection->getTokenLocationFileByPath(message->filePath);
				if (!params.locationFile)
				{
					return;
				}
				params.locationFile->isWholeCopy = true;
			}
			else
			{
				std::shared_ptr<TokenLocationFile> file =
					m_storageAccess->getTokenLocationsForFile(message->filePath.str());

				TokenLocationFile* activeLocations = m_collection->findTokenLocationFileByPath(message->filePath);
				if (activeLocations)
				{
					activeLocations->forEachTokenLocation(
						[&file](TokenLocation* location)
						{
							file->addTokenLocationAsPlainCopy(location);
						}
					);
				}

				params.locationFile = file;
			}

			getView()->addCodeSnippets(std::vector<CodeSnippetParams>(1, params), false);
		}

		view->setFileState(message->filePath, CodeView::FILE_MAXIMIZED);
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
		getView()->scrollToDefinition();
		m_scrollToDefinition = false;
	}

	if (m_scrollToValue != -1)
	{
		getView()->scrollToValue(m_scrollToValue, m_scrollInListMode);
		m_scrollToValue = -1;
	}
}

void CodeController::handleMessage(MessageScrollToLine* message)
{
	getView()->scrollToLine(message->filePath, message->line);

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
		m_collection = m_storageAccess->getErrorTokenLocations(&errors);
		std::vector<CodeSnippetParams> snippets = getSnippetsForCollection(m_collection);

		view->clear();
		view->setErrorInfos(errors);
		view->showCodeSnippets(snippets, std::vector<Id>(), !message->errorId);

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
	view->showCodeSnippets(getSnippetsForCollection(m_collection, true), std::vector<Id>(), true);

	showContents(message);
}

void CodeController::handleMessage(MessageShowScope* message)
{
	TRACE("code scope");

	std::shared_ptr<TokenLocationCollection> collection =
		m_storageAccess->getTokenLocationsForLocationIds(std::vector<Id>(1, message->scopeLocationId));

	TokenLocation* location = collection->findTokenLocationById(message->scopeLocationId);
	if (!location || !location->isScopeTokenLocation() || !location->getOtherTokenLocation())
	{
		LOG_ERROR("MessageShowScope did not contain a valid scope location id");
		return;
	}

	std::vector<CodeSnippetParams> snippets =
		getSnippetsForFile(collection->getTokenLocationFiles().begin()->second, true);
	if (snippets.size() != 1)
	{
		LOG_ERROR("MessageShowScope didn't result in one single snippet to be created");
		return;
	}

	if (message->showErrors)
	{
		snippets[0].locationFile = m_collection->getTokenLocationFileByPath(snippets[0].locationFile->getFilePath());
	}
	else
	{
		TokenLocationFile* activeLocations =
			m_collection->findTokenLocationFileByPath(snippets[0].locationFile->getFilePath());
		if (activeLocations)
		{
			std::shared_ptr<TokenLocationFile> file = snippets[0].locationFile;
			activeLocations->forEachTokenLocation(
				[&file](TokenLocation* location)
				{
					file->addTokenLocationAsPlainCopy(location);
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

std::vector<CodeSnippetParams> CodeController::getSnippetsForActiveTokenLocations(
	const TokenLocationCollection* collection, Id declarationId
) const {
	TRACE();

	std::vector<CodeSnippetParams> snippets;
	collection->forEachTokenLocationFile(
		[&](std::shared_ptr<TokenLocationFile> file) -> void
		{
			bool isDeclarationFile = false;
			bool isDefinitionFile = false;
			file->forEachTokenLocation(
				[&](TokenLocation* location)
				{
					if (location->getTokenId() == declarationId)
					{
						isDeclarationFile = true;

						if (location->getType() == LOCATION_SCOPE)
						{
							isDefinitionFile = true;
						}
					}
				}
			);

			CodeSnippetParams params;
			params.locationFile = file;
			params.refCount = file->getUnscopedStartTokenLocationCount();

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
	std::shared_ptr<TokenLocationCollection> collection, bool addTokenLocations
) const
{
	std::vector<CodeSnippetParams> snippets;

	collection->forEachTokenLocationFile(
		[&](std::shared_ptr<TokenLocationFile> file) -> void
		{
			CodeSnippetParams params;
			params.locationFile = file;
			params.refCount = file->getUnscopedStartTokenLocationCount();

			params.isCollapsed = true;
			snippets.push_back(params);
		}
	);

	addModificationTimes(snippets);

	return snippets;
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForFile(
		std::shared_ptr<TokenLocationFile> activeTokenLocations, bool addTokenLocations
) const
{
	TRACE();

	std::shared_ptr<TextAccess> textAccess = m_storageAccess->getFileContent(activeTokenLocations->getFilePath());
	std::shared_ptr<TokenLocationFile> scopeLocations
		= std::make_shared<TokenLocationFile>(activeTokenLocations->getFilePath().str());

	std::shared_ptr<TokenLocationFile> fileLocations =
		m_storageAccess->getTokenLocationsForFile(activeTokenLocations->getFilePath().str());

	fileLocations->forEachStartTokenLocation(
		[&](TokenLocation* startLoc) -> void
		{
			if (startLoc->getType() == LOCATION_SCOPE)
			{
				TokenLocation* endLoc = startLoc->getOtherTokenLocation();
				TokenLocation* scopeLoc = scopeLocations->addTokenLocation(
					startLoc->getId(),
					startLoc->getTokenId(),
					startLoc->getLineNumber(),
					startLoc->getColumnNumber(),
					endLoc->getLineNumber(),
					endLoc->getColumnNumber());
				scopeLoc->setType(LOCATION_SCOPE);
			}
		}
	);

	std::deque<SnippetMerger::Range> ranges;

	if (activeTokenLocations->isWholeCopy)
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
		activeTokenLocations->forEachStartTokenLocation(
			[&](TokenLocation* location)
			{
				buildMergerHierarchy(location, scopeLocations, fileScopedMerger, mergers);
			}
		);

		std::vector<SnippetMerger::Range> atomicRanges;
		m_storageAccess->getCommentLocationsInFile(activeTokenLocations->getFilePath())->forEachStartTokenLocation(
			[&](TokenLocation* location)
			{
				atomicRanges.push_back(SnippetMerger::Range(
					SnippetMerger::Border(location->getLineNumber(), false),
					SnippetMerger::Border(location->getOtherTokenLocation()->getLineNumber(), false)
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
		params.locationFile = activeTokenLocations;
		params.refCount = activeTokenLocations->getUnscopedStartTokenLocationCount();
		params.startLineNumber = std::max<int>(1, range.start.row - (range.start.strong ? 0 : snippetExpandRange));
		params.endLineNumber =
			std::min<int>(textAccess->getLineCount(), range.end.row + (range.end.strong ? 0 : snippetExpandRange));

		std::shared_ptr<TokenLocationFile> tempFile =
			fileLocations->getFilteredByLines(params.startLineNumber, params.endLineNumber);
		TokenLocationLine* firstUsedLine = nullptr;
		for (size_t i = params.startLineNumber; i <= params.endLineNumber && firstUsedLine == nullptr; i++)
		{
			firstUsedLine = tempFile->findTokenLocationLineByNumber(i);
		}

		params.titleId = 0;
		if (firstUsedLine && firstUsedLine->getTokenLocations().size())
		{
			getTokenLocationOfParentScope(
				firstUsedLine->getTokenLocations().begin()->second.get(),
				scopeLocations
			)->forEachStartTokenLocation( // this TokenLocationFile only contains a single StartTokenLocation.
				[&](TokenLocation* location)
				{
					params.title = m_storageAccess->getNameHierarchyForNodeWithId(location->getTokenId()).getQualifiedName();
					params.titleId = location->getId();
				}
			);
		}

		if (!activeTokenLocations->isWholeCopy && params.titleId == 0)
		{
			params.title = activeTokenLocations->getFilePath().str();
		}

		TokenLocationLine* lastUsedLine = nullptr;
		for (size_t i = params.endLineNumber; i >= params.startLineNumber && lastUsedLine == nullptr; i--)
		{
			lastUsedLine = tempFile->findTokenLocationLineByNumber(i);
		}

		params.footerId = 0;
		if (lastUsedLine && lastUsedLine->getTokenLocations().size())
		{
			getTokenLocationOfParentScope(
				lastUsedLine->getTokenLocations().begin()->second.get(),
				scopeLocations
			)->forEachStartTokenLocation( // this TokenLocationFile only contains a single StartTokenLocation.
				[&](TokenLocation* location)
				{
					params.footer = m_storageAccess->getNameHierarchyForNodeWithId(location->getTokenId()).getQualifiedName();
					params.footerId = location->getId();
				}
			);
		}

		for (const std::string& line: textAccess->getLines(params.startLineNumber, params.endLineNumber))
		{
			params.code += line;
		}

		snippets.push_back(params);
	}

	if (addTokenLocations && !activeTokenLocations->isWholeCopy)
	{
		for (CodeSnippetParams& params : snippets)
		{
			std::shared_ptr<TokenLocationFile> lines =
				fileLocations->getFilteredByLines(params.startLineNumber, params.endLineNumber);

			params.locationFile->forEachTokenLocation(
				[&lines](TokenLocation* location)
				{
					lines->addTokenLocationAsPlainCopy(location);
				}
			);

			params.locationFile = lines;
		}
	}

	return snippets;
}

std::shared_ptr<SnippetMerger> CodeController::buildMergerHierarchy(
	TokenLocation* location,
	std::shared_ptr<TokenLocationFile> scopeLocations,
	SnippetMerger& fileScopedMerger,
	std::map<int, std::shared_ptr<SnippetMerger>>& mergers
) const
{
	const TokenLocation* currentLocation = location;
	std::shared_ptr<SnippetMerger> currentMerger = std::make_shared<SnippetMerger>(
		currentLocation->getStartTokenLocation()->getLineNumber(),
		currentLocation->getEndTokenLocation()->getLineNumber()
	);

	std::shared_ptr<TokenLocationFile> locationFile = getTokenLocationOfParentScope(currentLocation, scopeLocations);
	if (locationFile->getTokenLocationLineCount() == 0)
	{
		fileScopedMerger.addChild(currentMerger);
		return currentMerger;
	}

	std::shared_ptr<SnippetMerger> nextMerger;
	locationFile->forEachStartTokenLocation( // contains just 1 start location
		[&](TokenLocation* scopeLocation)
		{
			std::map<int, std::shared_ptr<SnippetMerger>>::iterator it = mergers.find(scopeLocation->getId());
			if (it == mergers.end())
			{
				nextMerger = buildMergerHierarchy(scopeLocation, scopeLocations, fileScopedMerger, mergers);
				mergers[scopeLocation->getId()] = nextMerger;
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

std::shared_ptr<TokenLocationFile> CodeController::getTokenLocationOfParentScope(
	const TokenLocation* location,
	std::shared_ptr<TokenLocationFile> scopeLocations
) const
{
	const TokenLocation* parent = location;
	const FilePath filePath = location->getFilePath();

	scopeLocations->forEachStartTokenLocation(
		[&](TokenLocation* tokenLocation) -> void
		{
			if ((*tokenLocation) < *(location->getStartTokenLocation()) &&
				(*tokenLocation->getEndTokenLocation()) > *(location->getEndTokenLocation()))
			{
				if (parent == location)
				{
					parent = tokenLocation;
				}
				// since tokenLocation is a start location the > location indicates the scope
				// that is closer to the child.
				else if ((*tokenLocation) > *parent)
				{
					parent = tokenLocation;
				}
			}
		}
	);

	std::shared_ptr<TokenLocationFile> file = std::make_shared<TokenLocationFile>(filePath);
	if (parent != location)
	{
		file->addTokenLocationAsPlainCopy(parent);
		file->addTokenLocationAsPlainCopy(parent->getOtherTokenLocation());
	}
	return file;
}

std::vector<std::string> CodeController::getProjectDescription(TokenLocationFile* locationFile) const
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
			Id tokenId = m_storageAccess->getIdForNodeWithNameHierarchy(nameHierarchy);

			std::string nameString = nameHierarchy.getQualifiedName();
			if (tokenId > 0)
			{
				line.replace(posA, posB - posA + 1, nameString);
				locationFile->addTokenLocation(
					0, tokenId,
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
