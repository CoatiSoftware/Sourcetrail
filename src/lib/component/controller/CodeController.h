#ifndef CODE_CONTROLLER_H
#define CODE_CONTROLLER_H

#include <string>

#include "component/controller/Controller.h"
#include "component/view/CodeView.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokenLocation.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageShowFile.h"
#include "utility/types.h"

class GraphAccess;
class LocationAccess;
class TokenLocationFile;

class CodeController
	: public Controller
	, public MessageListener<MessageActivateTokenLocation>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageShowFile>
{
public:
	CodeController(GraphAccess* graphAccess, LocationAccess* locationAccess);
	~CodeController();

private:
	static const uint s_lineRadius;

	virtual void handleMessage(MessageActivateTokenLocation* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageShowFile* message);

	CodeView* getView();

	std::vector<CodeView::CodeSnippetParams> getSnippetsForActiveTokenIds(
		const std::vector<Id>& ids, Id declarationId) const;
	std::vector<std::pair<uint, uint>> getSnippetRangesForFile(TokenLocationFile* file, const uint lineRadius) const;

	GraphAccess* m_graphAccess;
	LocationAccess* m_locationAccess;
};

#endif // CODE_CONTROLLER_H
