#ifndef CODE_CONTROLLER_H
#define CODE_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateToken.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/types.h"

class CodeView;
class LocationAccess;

struct AnnotatedText
{
	std::string text;
	Id tokenId;
};

class CodeController
	: public Controller
	, public MessageListener<MessageActivateToken>
	, public MessageListener<MessageRefresh>
{
public:
	CodeController(LocationAccess* locationAccess);
	~CodeController();

	void setActiveTokenId(Id id);

private:
	virtual void handleMessage(MessageActivateToken* message);
	virtual void handleMessage(MessageRefresh* message);
	CodeView* getView();

	LocationAccess* m_locationAccess;
};

#endif // CODE_CONTROLLER_H
