#include "component/controller/CodeController.h"

#include "component/view/CodeView.h"
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

void CodeController::setActiveTokenId(Id id)
{
	const uint lineRadius = 2;

	getView()->clearCodeSnippets();

	std::vector<Id> activeTokenIds = m_graphAccess->getActiveTokenIdsForId(id);
	std::vector<Id> locationIds = m_graphAccess->getLocationIdsForTokenIds(activeTokenIds);

	TokenLocationCollection collection = m_locationAccess->getTokenLocationsForLocationIds(locationIds);
	collection.forEachTokenLocationFile(
		[&](TokenLocationFile* file) -> void
		{
			const std::string filePath = file->getFilePath();
			std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

			std::vector<std::pair<uint, uint>> ranges = getSnippetRangesForFile(file, lineRadius);
			for (const std::pair<uint, uint>& range: ranges)
			{
				unsigned int firstLineNumber = std::max<int>(1, range.first - lineRadius);
				unsigned int lastLineNumber = std::min<int>(textAccess->getLineCount(), range.second + lineRadius);

				CodeView::CodeSnippetParams params;
				for (const std::string& line: textAccess->getLines(firstLineNumber, lastLineNumber))
				{
					params.code += line;
				}

				params.startLineNumber = firstLineNumber;
				params.locationFile =
					m_locationAccess->getTokenLocationsForLinesInFile(filePath, firstLineNumber, lastLineNumber);
				params.activeTokenIds = activeTokenIds;

				getView()->addCodeSnippet(params);
			}
		}
	);
}

void CodeController::handleMessage(MessageActivateToken* message)
{
	setActiveTokenId(message->tokenId);
}

void CodeController::handleMessage(MessageRefresh* message)
{
	getView()->refreshView();
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
