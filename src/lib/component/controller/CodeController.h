#ifndef CODE_CONTROLLER_H
#define CODE_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokenLocation.h"
#include "utility/types.h"

class CodeView;
class LocationAccess;

struct AnnotatedText
{
	std::string text;
	Id tokenId;
};

class CodeController: public Controller, public MessageListener<MessageActivateTokenLocation>
{
public:
	CodeController(std::shared_ptr<LocationAccess> locationAccess);
	~CodeController();

	void setActiveTokenLocationId(Id id);

private:
	virtual void handleMessage(MessageActivateTokenLocation* message);
	CodeView* getView();

	std::shared_ptr<LocationAccess> m_locationAccess;
};


#endif // CODE_CONTROLLER_H
