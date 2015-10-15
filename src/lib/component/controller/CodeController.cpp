#include "component/controller/CodeController.h"

#include <memory>

#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"

#include "data/access/StorageAccess.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "settings/ApplicationSettings.h"

CodeController::CodeController(StorageAccess* storageAccess)
	: m_storageAccess(storageAccess)
{
}

CodeController::~CodeController()
{
}

const uint CodeController::s_lineRadius = 2;

void CodeController::handleMessage(MessageActivateTokens* message)
{
	if (message->keepContent() && message->isIgnorable())
	{
		return;
	}

	std::vector<Id> activeTokenIds = message->tokenIds;
	Id declarationId = 0; // 0 means that no token is found.

	if (activeTokenIds.size() == 1)
	{
		activeTokenIds = m_storageAccess->getActiveTokenIdsForId(activeTokenIds[0], &declarationId);
	}
	// TODO: what about declarationId if more than 1 token is active? FIX THIS!

	CodeView* view = getView();
	view->setActiveTokenIds(activeTokenIds);
	view->setErrorMessages(std::vector<std::string>());

	if (message->isEdge)
	{
		view->showFirstActiveSnippet();
	}

	if (message->keepContent())
	{
		return;
	}

	std::shared_ptr<TokenLocationCollection> collection = m_storageAccess->getTokenLocationsForTokenIds(activeTokenIds);
	view->showCodeSnippets(getSnippetsForActiveTokenLocations(collection.get(), declarationId));

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

void CodeController::handleMessage(MessageFinishedParsing* message)
{
	if (message->errorCount > 0)
	{
		std::vector<std::string> errorMessages;
		TokenLocationCollection errorCollection = m_storageAccess->getErrorTokenLocations(&errorMessages);

		std::vector<CodeView::CodeSnippetParams> snippets;

		errorCollection.forEachTokenLocationFile(
			[&](std::shared_ptr<TokenLocationFile> file) -> void
			{
				std::vector<CodeView::CodeSnippetParams> fileSnippets = getSnippetsForFile(file);
				snippets.insert(snippets.end(), fileSnippets.begin(), fileSnippets.end());
			}
		);

		CodeView* view = getView();
		view->setActiveTokenIds(std::vector<Id>());
		view->setErrorMessages(errorMessages);
		view->showCodeSnippets(snippets);
	}
}

void CodeController::handleMessage(MessageFocusIn* message)
{
	getView()->focusToken(message->tokenId);
}

void CodeController::handleMessage(MessageFocusOut* message)
{
	getView()->defocusToken();
}

void CodeController::handleMessage(MessageShowFile* message)
{
	CodeView::CodeSnippetParams params;
	params.startLineNumber = message->startLineNumber;
	params.endLineNumber = message->endLineNumber;

	std::shared_ptr<TextAccess> textAccess = m_storageAccess->getFileContent(message->filePath);
	params.code = textAccess->getText();

	params.locationFile = m_storageAccess->getTokenLocationsForFile(message->filePath.str());

	getView()->showCodeFile(params);
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

	std::vector<CodeView::CodeSnippetParams> snippets = getSnippetsForActiveTokenLocations(collection.get(), 0);

	if (snippets.size() != 1)
	{
		LOG_ERROR("MessageShowScope didn't result in one single snippet to be created");
		return;
	}

	getView()->addCodeSnippets(snippets);
}

void CodeController::handleMessage(MessageShowSnippets* message)
{
	getView()->addCodeSnippets(getSnippetsForActiveTokenLocationsInFile(message->locationFile));
}

CodeView* CodeController::getView()
{
	return Controller::getView<CodeView>();
}

std::vector<CodeView::CodeSnippetParams> CodeController::getSnippetsForActiveTokenLocations(
	const TokenLocationCollection* collection, Id declarationId
) const {
	std::vector<CodeView::CodeSnippetParams> snippets;

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
				std::vector<CodeView::CodeSnippetParams> fileSnippets = getSnippetsForActiveTokenLocationsInFile(file);

				for (CodeView::CodeSnippetParams& snippet : fileSnippets)
				{
					snippet.isDeclaration = isDeclarationFile;
				}

				utility::append(snippets, fileSnippets);
			}
			else
			{
				CodeView::CodeSnippetParams params;
				params.locationFile = file;
				params.refCount = file->getUnscopedStartTokenLocationCount();

				params.isCollapsed = true;
				snippets.push_back(params);
			}
		}
	);

	std::sort(snippets.begin(), snippets.end(), CodeView::CodeSnippetParams::sort);

	return snippets;
}

std::vector<CodeView::CodeSnippetParams> CodeController::getSnippetsForActiveTokenLocationsInFile(
	std::shared_ptr<TokenLocationFile> file
) const {
	std::vector<CodeView::CodeSnippetParams> fileSnippets = getSnippetsForFile(file);

	if (!file->isWholeCopy)
	{
		for (CodeView::CodeSnippetParams& params : fileSnippets)
		{
			params.locationFile = m_storageAccess->getTokenLocationsForLinesInFile(
				file->getFilePath().str(), params.startLineNumber, params.endLineNumber);
		}
	}

	return fileSnippets;
}

std::vector<CodeView::CodeSnippetParams> CodeController::getSnippetsForFile(std::shared_ptr<TokenLocationFile> file) const
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

		ranges = fileScopedMerger.merge();
	}

	const int snippetExpandRange = ApplicationSettings::getInstance()->getCodeSnippetExpandRange();
	std::vector<CodeView::CodeSnippetParams> snippets;
	for (const SnippetMerger::Range& range: ranges)
	{
		CodeView::CodeSnippetParams params;
		params.locationFile = file;
		params.refCount = file->getUnscopedStartTokenLocationCount();
		params.startLineNumber = std::max<int>(1, range.start.row - (range.start.strong ? 0 : snippetExpandRange));
		params.endLineNumber = std::min<int>(textAccess->getLineCount(), range.end.row + (range.end.strong ? 0 : snippetExpandRange));

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
