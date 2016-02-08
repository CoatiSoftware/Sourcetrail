#include "component/controller/CodeController.h"

#include <memory>

#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

#include "data/access/StorageAccess.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "settings/ApplicationSettings.h"
#include "settings/ProjectSettings.h"

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
	std::vector<std::string> errorMessages;
	std::vector<CodeSnippetParams> snippets = getSnippetsForErrorLocations(&errorMessages);

	StorageStats stats = m_storageAccess->getStorageStats();
	CodeSnippetParams statsSnippet;

	statsSnippet.startLineNumber = 1;
	statsSnippet.endLineNumber = 1;

	statsSnippet.reduced = true;

	statsSnippet.locationFile = std::make_shared<TokenLocationFile>(FilePath());
	statsSnippet.locationFile->isWholeCopy = true;

	std::vector<std::string> description = getProjectDescription(statsSnippet.locationFile.get());

	std::stringstream ss;

	ss << "\n";
	ss << "\t" + ProjectSettings::getInstance()->getFilePath().withoutExtension().fileName() + "\n";

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
	ss << "\t" + std::to_string(stats.errorCount) + " errors\n";
	ss << "\n";

	if (stats.errorCount > 0)
	{
		ss << "\tWarning: The analysis may be incomplete as long as it yields errors.\n";
		ss << "\tTry resolving them and refresh the project.\n";
		ss << "\n";
	}

	statsSnippet.code = ss.str();

	snippets.insert(snippets.begin(), statsSnippet);

	CodeView* view = getView();
	view->setErrorMessages(errorMessages);
	view->showCodeSnippets(snippets, std::vector<Id>());

	showContents(message);
}

void CodeController::handleMessage(MessageActivateTokens* message)
{
	std::vector<Id> activeTokenIds = message->tokenIds;
	Id declarationId = 0; // 0 means that no token is found.

	if (activeTokenIds.size() == 1)
	{
		activeTokenIds = m_storageAccess->getActiveTokenIdsForId(activeTokenIds[0], &declarationId);
	}
	// TODO: what about declarationId if more than 1 token is active? FIX THIS!

	CodeView* view = getView();
	view->setErrorMessages(std::vector<std::string>());

	if (message->isEdge)
	{
		view->showFirstActiveSnippet(activeTokenIds, message->isLast());
	}

	if (message->keepContent())
	{
		view->showActiveTokenIds(activeTokenIds);
	}
	else
	{
		std::shared_ptr<TokenLocationCollection> collection = m_storageAccess->getTokenLocationsForTokenIds(activeTokenIds);
		view->showCodeSnippets(getSnippetsForActiveTokenLocations(collection.get(), declarationId), activeTokenIds);

		if (!message->isFromSystem)
		{
			size_t fileCount = collection->getTokenLocationFileCount();
			size_t referenceCount = collection->getTokenLocationCount();

			std::stringstream ss;
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
	}

	showContents(message);
}

void CodeController::handleMessage(MessageChangeFileView* message)
{
	CodeView* view = getView();

	switch (message->state)
	{
	case MessageChangeFileView::FILE_MINIMIZED:
		view->setFileState(message->filePath, CodeView::FILE_MINIMIZED);
		break;

	case MessageChangeFileView::FILE_SNIPPETS:
		if (message->needsData)
		{
			view->addCodeSnippets(getSnippetsForActiveTokenLocationsInFile(message->locationFile), false);
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

			params.modificationTime = m_storageAccess->getFileModificationTime(message->filePath);

			if (message->showErrors)
			{
				std::vector<std::string> errorMessages;
				TokenLocationCollection errorCollection = m_storageAccess->getErrorTokenLocations(&errorMessages);
				params.locationFile = std::make_shared<TokenLocationFile>(*errorCollection.findTokenLocationFileByPath(message->filePath));
				params.locationFile->isWholeCopy = true;
			}
			else
			{
				params.locationFile = m_storageAccess->getTokenLocationsForFile(message->filePath.str());
			}

			getView()->showCodeFile(params);
		}
		view->setFileState(message->filePath, CodeView::FILE_MAXIMIZED);
		break;
	}

	showContents(message);
}

void CodeController::handleMessage(MessageFlushUpdates* message)
{
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

void CodeController::handleMessage(MessageScrollCode* message)
{
	if (!message->isFresh())
	{
		getView()->scrollToValue(message->value);
	}
}

void CodeController::handleMessage(MessageShowErrors* message)
{
	std::vector<std::string> errorMessages;
	std::vector<CodeSnippetParams> snippets = getSnippetsForErrorLocations(&errorMessages);

	CodeView* view = getView();
	view->setErrorMessages(errorMessages);
	view->showCodeSnippets(snippets, std::vector<Id>());

	showContents(message);
}

void CodeController::handleMessage(MessageShowScope* message)
{
	std::shared_ptr<TokenLocationCollection> collection =
		m_storageAccess->getTokenLocationsForLocationIds(std::vector<Id>(1, message->scopeLocationId));

	TokenLocation* location = collection->findTokenLocationById(message->scopeLocationId);
	if (!location || !location->isScopeTokenLocation() || !location->getOtherTokenLocation())
	{
		LOG_ERROR("MessageShowScope did not contain a valid scope location id");
		return;
	}

	std::vector<CodeSnippetParams> snippets = getSnippetsForActiveTokenLocations(collection.get(), 0);

	if (snippets.size() != 1)
	{
		LOG_ERROR("MessageShowScope didn't result in one single snippet to be created");
		return;
	}

	getView()->addCodeSnippets(snippets, true);

	showContents(message);
}

CodeView* CodeController::getView()
{
	return Controller::getView<CodeView>();
}

void CodeController::showContents(MessageBase* message)
{
	if (message->undoRedoType == MessageBase::UNDOTYPE_NORMAL)
	{
		getView()->showContents();
	}
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForActiveTokenLocations(
	const TokenLocationCollection* collection, Id declarationId
) const {
	std::vector<CodeSnippetParams> snippets;

	collection->forEachTokenLocationFile(
		[&](std::shared_ptr<TokenLocationFile> file) -> void
		{
			bool isDeclarationFile = false;
			file->forEachTokenLocation(
				[&](TokenLocation* location)
				{
					if (location->getTokenId() == declarationId)
					{
						isDeclarationFile = true;
					}
				}
			);

			if (isDeclarationFile || collection->getTokenLocationFileCount() < 5 || file->isWholeCopy)
			{
				std::vector<CodeSnippetParams> fileSnippets = getSnippetsForActiveTokenLocationsInFile(file);

				for (CodeSnippetParams& snippet : fileSnippets)
				{
					snippet.isDeclaration = isDeclarationFile;
				}

				utility::append(snippets, fileSnippets);
			}
			else
			{
				CodeSnippetParams params;
				params.locationFile = file;
				params.refCount = file->getUnscopedStartTokenLocationCount();
				params.modificationTime = m_storageAccess->getFileModificationTime(file->getFilePath());

				params.isCollapsed = true;
				snippets.push_back(params);
			}
		}
	);

	std::sort(snippets.begin(), snippets.end(), CodeSnippetParams::sort);

	return snippets;
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForActiveTokenLocationsInFile(
	std::shared_ptr<TokenLocationFile> file
) const {
	std::vector<CodeSnippetParams> fileSnippets = getSnippetsForFile(file);

	if (!file->isWholeCopy)
	{
		for (CodeSnippetParams& params : fileSnippets)
		{
			params.locationFile = m_storageAccess->getTokenLocationsForLinesInFile(
				file->getFilePath().str(), params.startLineNumber, params.endLineNumber);
		}
	}

	return fileSnippets;
}

std::vector<CodeSnippetParams> CodeController::getSnippetsForFile(std::shared_ptr<TokenLocationFile> file) const
{
	std::shared_ptr<TextAccess> textAccess = m_storageAccess->getFileContent(file->getFilePath());

	std::deque<SnippetMerger::Range> ranges;

	if (file->isWholeCopy)
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
		file->forEachStartTokenLocation(
			[&](TokenLocation* location)
			{
				buildMergerHierarchy(location, fileScopedMerger, mergers);
			}
		);

		std::vector<SnippetMerger::Range> atomicRanges;
		m_storageAccess->getCommentLocationsInFile(file->getFilePath())->forEachStartTokenLocation(
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
		params.locationFile = file;
		params.refCount = file->getUnscopedStartTokenLocationCount();
		params.startLineNumber = std::max<int>(1, range.start.row - (range.start.strong ? 0 : snippetExpandRange));
		params.endLineNumber = std::min<int>(textAccess->getLineCount(), range.end.row + (range.end.strong ? 0 : snippetExpandRange));
		params.modificationTime = m_storageAccess->getFileModificationTime(file->getFilePath());


		std::shared_ptr<TokenLocationFile> tempFile =
			m_storageAccess->getTokenLocationsForLinesInFile(file->getFilePath().str(), params.startLineNumber, params.endLineNumber);
		TokenLocationLine* firstUsedLine = nullptr;
		for (size_t i = params.startLineNumber; i <= params.endLineNumber && firstUsedLine == nullptr; i++)
		{
			firstUsedLine = tempFile->findTokenLocationLineByNumber(i);
		}

		params.titleId = 0;
		if (firstUsedLine && firstUsedLine->getTokenLocations().size())
		{
			m_storageAccess->getTokenLocationOfParentScope(
				firstUsedLine->getTokenLocations().begin()->second.get()
			)->forEachStartTokenLocation( // this TokenLocationFile only contains a single StartTokenLocation.
				[&](TokenLocation* location)
				{
					params.title = m_storageAccess->getNameHierarchyForNodeWithId(location->getTokenId()).getFullName();
					params.titleId = location->getId();
				}
			);
		}
		if (!file->isWholeCopy && params.titleId == 0)
		{
			params.title = file->getFilePath().str();
		}


		TokenLocationLine* lastUsedLine = nullptr;
		for (size_t i = params.endLineNumber; i >= params.startLineNumber && lastUsedLine == nullptr; i--)
		{
			lastUsedLine = tempFile->findTokenLocationLineByNumber(i);
		}

		params.footerId = 0;
		if (lastUsedLine && lastUsedLine->getTokenLocations().size())
		{
			m_storageAccess->getTokenLocationOfParentScope(
				lastUsedLine->getTokenLocations().begin()->second.get()
			)->forEachStartTokenLocation( // this TokenLocationFile only contains a single StartTokenLocation.
				[&](TokenLocation* location)
				{
					params.footer = m_storageAccess->getNameHierarchyForNodeWithId(location->getTokenId()).getFullName();
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

	return snippets;
}

std::shared_ptr<SnippetMerger> CodeController::buildMergerHierarchy(
	TokenLocation* location, SnippetMerger& fileScopedMerger, std::map<int, std::shared_ptr<SnippetMerger>>& mergers) const
{
	const TokenLocation* currentLocation = location;
	std::shared_ptr<SnippetMerger> currentMerger = std::make_shared<SnippetMerger>(
		currentLocation->getStartTokenLocation()->getLineNumber(),
		currentLocation->getEndTokenLocation()->getLineNumber()
	);

	std::shared_ptr<TokenLocationFile> locationFile = m_storageAccess->getTokenLocationOfParentScope(currentLocation);
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
				nextMerger = buildMergerHierarchy(scopeLocation, fileScopedMerger, mergers);
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

std::vector<CodeSnippetParams> CodeController::getSnippetsForErrorLocations(
	std::vector<std::string>* errorMessages)
const {
	TokenLocationCollection errorCollection = m_storageAccess->getErrorTokenLocations(errorMessages);

	std::vector<CodeSnippetParams> snippets;

	errorCollection.forEachTokenLocationFile(
		[&](std::shared_ptr<TokenLocationFile> file) -> void
		{
			std::vector<CodeSnippetParams> fileSnippets = getSnippetsForFile(file);
			snippets.insert(snippets.end(), fileSnippets.begin(), fileSnippets.end());
		}
	);

	return snippets;
}

std::vector<std::string> CodeController::getProjectDescription(TokenLocationFile* locationFile) const
{
	std::string description = ProjectSettings::getInstance()->getDescription();

	if (!description.size())
	{
		return std::vector<std::string>();
	}

	std::vector<std::string> lines = utility::splitToVector(description, "\\n");
	size_t startLineNumber = 4;

	for (size_t i = 0; i < lines.size(); i++)
	{
		std::string line = "\t" + lines[i];

		size_t pos = 0;

		while (pos != std::string::npos)
		{
			size_t posA = line.find('[', pos);
			size_t posB = line.find(']', posA);

			if (posA == std::string::npos || posB == std::string::npos)
			{
				break;
			}

			std::string tokenName = line.substr(posA + 1, posB - posA - 1);

			Id tokenId = m_storageAccess->getIdForNodeWithSearchNameHierarchy(NameHierarchy(tokenName));

			if (tokenId > 0)
			{
				line.replace(posA, posB - posA + 1, tokenName);
				locationFile->addTokenLocation(
					0, tokenId,
					startLineNumber + i, posA + 1,
					startLineNumber + i, posA + tokenName.size()
				);
			}

			pos = posA + tokenName.size();
		}

		lines[i] = line;
	}

	return lines;
}
