#include "component/controller/CodeController.h"

#include "component/view/CodeView.h"
#include "data/access/LocationAccess.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"
#include "utility/text/TextAccess.h"

#include "utility/logging/logging.h"


CodeController::CodeController(LocationAccess* locationAccess)
	: m_locationAccess(locationAccess)
{
}

CodeController::~CodeController()
{
}

void CodeController::setActiveTokenId(Id id)
{
	const unsigned int lineRadius = 2;

	getView()->clearCodeSnippets();

	TokenLocationCollection collection = m_locationAccess->getTokenLocationsForTokenId(id);
	collection.forEachTokenLocation(
		[&](TokenLocation* tokenLocation) -> void
		{
			if (tokenLocation->isStartTokenLocation())
			{
				const std::string filePath = tokenLocation->getFilePath();
				std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(filePath);

				unsigned int firstLineNumber = std::max<int>(1, tokenLocation->getLineNumber() - lineRadius);
				unsigned int lastLineNumber = std::min<int>(
					textAccess->getLineCount(), tokenLocation->getEndTokenLocation()->getLineNumber() + lineRadius
				);

				CodeView::CodeSnippetParams params;
				for (const std::string& line: textAccess->getLines(firstLineNumber, lastLineNumber))
				{
					params.code += line;
				}

				params.startLineNumber = firstLineNumber;
				params.locationFile =
					m_locationAccess->getTokenLocationsForLinesInFile(filePath, firstLineNumber, lastLineNumber);
				params.activeTokenId = id;

				getView()->addCodeSnippet(params);
			}
		}
	);
}

void CodeController::handleMessage(MessageActivateToken* message)
{
	setActiveTokenId(message->tokenId);
}

CodeView* CodeController::getView()
{
	return Controller::getView<CodeView>();
}
