#include "component/controller/CodeController.h"

#include "component/view/CodeView.h"
#include "data/access/LocationAccess.h"
#include "data/location/TokenLocation.h"
#include "utility/text/TextAccess.h"

#include "utility/logging/logging.h"


CodeController::CodeController(std::shared_ptr<LocationAccess> locationAccess)
	: m_locationAccess(locationAccess)
{
}

CodeController::~CodeController()
{
}

void CodeController::setActiveTokenLocationId(Id id)
{
	TokenLocation* tokenLocation = m_locationAccess->getTokenLocation(id);
	if (tokenLocation)
	{
		getView()->clearCodeSnippets();
		std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(tokenLocation->getFilePath());
		getView()->addCodeSnippet(textAccess->getText());
	}
}

void CodeController::handleMessage(MessageActivateTokenLocation* message)
{
	setActiveTokenLocationId(1);
}

CodeView* CodeController::getView()
{
	return Controller::getView<CodeView>();
}
