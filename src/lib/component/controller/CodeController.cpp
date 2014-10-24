#include "component/controller/CodeController.h"

#include "component/view/CodeView.h"
#include "data/access/GraphAccess.h"
#include "data/access/LocationAccess.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"
#include "utility/text/TextAccess.h"

const uint CodeController::s_lineRadius = 2;

CodeController::CodeController(GraphAccess* graphAccess, LocationAccess* locationAccess)
	: m_graphAccess(graphAccess)
	, m_locationAccess(locationAccess)
{
}

CodeController::~CodeController()
{
}

void CodeController::setActiveTokenIds(const std::vector<Id>& ids, Id activeId, Id declarationId)
{
	getView()->clearCodeSnippets();
	getView()->setActiveTokenIds(ids);

	std::vector<Id> locationIds = m_graphAccess->getLocationIdsForTokenIds(ids);

	std::vector<CodeView::CodeSnippetParams> snippets;

	TokenLocationCollection collection = m_locationAccess->getTokenLocationsForLocationIds(locationIds);

	collection.forEachTokenLocationFile(
		[&](TokenLocationFile* file) -> void
		{
			const std::string filePath = file->getFilePath();
			std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

			std::vector<std::pair<uint, uint>> ranges = getSnippetRangesForFile(file, s_lineRadius);

			for (const std::pair<uint, uint>& range: ranges)
			{
				unsigned int firstLineNumber = std::max<int>(1, range.first - s_lineRadius);
				unsigned int lastLineNumber = std::min<int>(textAccess->getLineCount(), range.second + s_lineRadius);

				CodeView::CodeSnippetParams params;
				for (const std::string& line: textAccess->getLines(firstLineNumber, lastLineNumber))
				{
					params.code += line;
				}

				params.startLineNumber = firstLineNumber;
				params.locationFile =
					m_locationAccess->getTokenLocationsForLinesInFile(filePath, firstLineNumber, lastLineNumber);	
				params.locationFile.forEachTokenLocation(
					[&](TokenLocation* location)
				{
					if(location->getTokenId() == activeId && activeId != 0)
					{
						params.isActive = true;
					}
					if(location->getTokenId() == declarationId && declarationId != 0)
					{
						params.isDeclaration = true;
					}					
				}	
				);

				snippets.push_back(params);				
			}
		}
	);
	
	std::sort(snippets.begin(), snippets.end(), CodeView::CodeSnippetParams::sort);

	for( CodeView::CodeSnippetParams p : snippets)
	{		
		getView()->addCodeSnippet(p);			
	}
}

void CodeController::handleMessage(MessageActivateToken* message)
{
	Id declarationId;
	std::vector<Id> activeTokenIds = m_graphAccess->getActiveTokenIdsForId(message->tokenId, declarationId);
	setActiveTokenIds(activeTokenIds, message->tokenId, declarationId);
}

void CodeController::handleMessage(MessageActivateTokens* message)
{
	if (message->tokenIds.size	() == 1)
	{
		Id declarationId;
		std::vector<Id> activeTokenIds = m_graphAccess->getActiveTokenIdsForId(message->tokenIds[0], declarationId);
		setActiveTokenIds(activeTokenIds, message->tokenIds[0], declarationId);
	}
	else
	{
		setActiveTokenIds(message->tokenIds, 0, 0);
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
	getView()->setActiveTokenIds(message->activeTokenIds);

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

std::vector<std::pair<uint, uint>> CodeController::getSnippetRangesForFile(
	TokenLocationFile* file, const uint lineRadius
) const
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
				if (!start)
				{
					start = lineNumber;
				}
				else if (end && lineNumber > end + 2 * lineRadius + 1)
				{
					ranges.push_back(std::make_pair(uint(start), uint(end)));
					start = lineNumber;
					end = 0;
				}
			}
			else
			{
				end = lineNumber;
			}
		}
	);

	ranges.push_back(std::make_pair(uint(start), uint(end)));
	return ranges;
}
