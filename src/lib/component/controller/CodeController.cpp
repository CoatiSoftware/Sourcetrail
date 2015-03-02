#include "component/controller/CodeController.h"

#include "data/access/GraphAccess.h"
#include "data/access/LocationAccess.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"
#include "utility/text/TextAccess.h"

CodeController::CodeController(GraphAccess* graphAccess, LocationAccess* locationAccess)
	: m_graphAccess(graphAccess)
	, m_locationAccess(locationAccess)
{
}

CodeController::~CodeController()
{
}

const uint CodeController::s_lineRadius = 2;

void CodeController::handleMessage(MessageActivateTokenLocation* message)
{
	if (message->locationId)
	{
		std::vector<Id> activeTokenIds = m_graphAccess->getActiveTokenIdsForLocationId(message->locationId);
		MessageActivateTokens(activeTokenIds).dispatch();
	}
}

void CodeController::handleMessage(MessageActivateTokens* message)
{
	std::vector<Id> activeTokenIds = message->tokenIds;
	Id declarationId = 0;

	if (activeTokenIds.size() == 1)
	{
		activeTokenIds = m_graphAccess->getActiveTokenIdsForId(activeTokenIds[0], &declarationId);
	}

	CodeView* view = getView();
	view->setActiveTokenIds(activeTokenIds);
	view->setErrorMessages(std::vector<std::string>());
	view->showCodeSnippets(getSnippetsForActiveTokenIds(activeTokenIds, declarationId));
}

void CodeController::handleMessage(MessageFinishedParsing* message)
{
	if (message->errorCount > 0)
	{
		std::vector<std::string> errorMessages;
		TokenLocationCollection errorCollection = m_locationAccess->getErrorTokenLocations(&errorMessages);

		std::vector<CodeView::CodeSnippetParams> snippets;

		errorCollection.forEachTokenLocationFile(
			[&](TokenLocationFile* file) -> void
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

void CodeController::handleMessage(MessageRefresh* message)
{
	getView()->refreshView();
}

void CodeController::handleMessage(MessageShowFile* message)
{
	CodeView::CodeSnippetParams params;
	params.startLineNumber = message->startLineNumber;
	params.endLineNumber = message->endLineNumber;

	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(message->filePath);
	params.lineCount = textAccess->getLineCount();
	params.code = textAccess->getText();

	params.locationFile = m_locationAccess->getTokenLocationsForFile(message->filePath);

	getView()->showCodeFile(params);
}

CodeView* CodeController::getView()
{
	return Controller::getView<CodeView>();
}

std::vector<CodeView::CodeSnippetParams> CodeController::getSnippetsForActiveTokenIds(
	const std::vector<Id>& ids, Id declarationId
) const {
	TokenLocationCollection collection = m_locationAccess->getTokenLocationsForTokenIds(ids);

	std::vector<CodeView::CodeSnippetParams> snippets;

	collection.forEachTokenLocationFile(
		[&](TokenLocationFile* file) -> void
		{
			std::vector<CodeView::CodeSnippetParams> fileSnippets = getSnippetsForFile(file);

			for (CodeView::CodeSnippetParams& params : fileSnippets)
			{
				params.locationFile = m_locationAccess->getTokenLocationsForLinesInFile(
					file->getFilePath(), params.startLineNumber, params.endLineNumber);
			}

			if (declarationId != 0)
			{
				bool isDeclarationFile = false;
				for (const CodeView::CodeSnippetParams& snippet : fileSnippets)
				{
					snippet.locationFile.forEachTokenLocation(
						[&](TokenLocation* location)
						{
							if (location->getTokenId() == declarationId)
							{
								isDeclarationFile = true;
							}
						}
					);
				}

				for (CodeView::CodeSnippetParams& snippet : fileSnippets)
				{
					snippet.isDeclaration = isDeclarationFile;
				}
			}

			snippets.insert(snippets.end(), fileSnippets.begin(), fileSnippets.end());
		}
	);

	std::sort(snippets.begin(), snippets.end(), CodeView::CodeSnippetParams::sort);

	return snippets;
}

std::vector<CodeView::CodeSnippetParams> CodeController::getSnippetsForFile(const TokenLocationFile* file) const
{
	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(file->getFilePath());

	std::vector<std::pair<uint, uint>> ranges = getSnippetRangesForFile(file);
	std::vector<CodeView::CodeSnippetParams> snippets;

	for (const std::pair<uint, uint>& range: ranges)
	{
		CodeView::CodeSnippetParams params;
		params.locationFile = *file;
		params.startLineNumber = std::max<int>(1, range.first - s_lineRadius);
		params.endLineNumber = std::min<int>(textAccess->getLineCount(), range.second + s_lineRadius);

		for (const std::string& line: textAccess->getLines(params.startLineNumber, params.endLineNumber))
		{
			params.code += line;
		}

		snippets.push_back(params);
	}

	return snippets;
}

std::vector<std::pair<uint, uint>> CodeController::getSnippetRangesForFile(const TokenLocationFile* file) const
{
	std::vector<std::pair<uint, uint>> ranges;
	uint start = 0;
	uint end = 0;

	file->forEachTokenLocation(
		[&](TokenLocation* location) -> void
		{
			uint lineNumber = location->getLineNumber();

			if (location->isStartTokenLocation())
			{
				if (start && end && lineNumber > end + 2 * s_lineRadius + 1)
				{
					ranges.push_back(std::make_pair(uint(start), uint(end)));
					start = end = 0;
				}

				if (!start)
				{
					start = lineNumber;
				}

				lineNumber = location->getEndTokenLocation()->getLineNumber();
			}

			if (lineNumber > end)
			{
				end = lineNumber;
			}
		}
	);

	ranges.push_back(std::make_pair(uint(start), uint(end)));
	return ranges;
}
