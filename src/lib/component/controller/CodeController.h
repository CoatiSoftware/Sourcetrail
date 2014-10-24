#ifndef CODE_CONTROLLER_H
#define CODE_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateToken.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageShowFile.h"
#include "utility/types.h"

class CodeView;
class GraphAccess;
class LocationAccess;
class TokenLocationFile;

struct AnnotatedText
{
	std::string text;
	Id tokenId;
};

class CodeController
	: public Controller
	, public MessageListener<MessageActivateToken>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageShowFile>
{
public:
	CodeController(GraphAccess* graphAccess, LocationAccess* locationAccess);
	~CodeController();

	void setActiveTokenIds(const std::vector<Id>& ids, Id activeId, Id declarationId);

private:
	virtual void handleMessage(MessageActivateToken* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageShowFile* message);

	CodeView* getView();

	static const uint s_lineRadius;

	std::vector<std::pair<uint, uint>> getSnippetRangesForFile(TokenLocationFile* file, const uint lineRadius) const;

	GraphAccess* m_graphAccess;
	LocationAccess* m_locationAccess;
};

#endif // CODE_CONTROLLER_H
