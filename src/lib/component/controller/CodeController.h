#ifndef CODE_CONTROLLER_H
#define CODE_CONTROLLER_H

#include <map>
#include <string>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageChangeFileView.h"
#include "utility/messaging/type/MessageFlushUpdates.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageScrollCode.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageShowScope.h"
#include "utility/types.h"

#include "component/controller/helper/SnippetMerger.h"
#include "component/controller/Controller.h"
#include "component/view/CodeView.h"

class StorageAccess;
class TokenLocationCollection;
class TokenLocationFile;

class CodeController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageChangeFileView>
	, public MessageListener<MessageFlushUpdates>
	, public MessageListener<MessageFocusIn>
	, public MessageListener<MessageFocusOut>
	, public MessageListener<MessageScrollCode>
	, public MessageListener<MessageShowErrors>
	, public MessageListener<MessageShowScope>
{
public:
	CodeController(StorageAccess* storageAccess);
	~CodeController();

private:
	static const uint s_lineRadius;

	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageChangeFileView* message);
	virtual void handleMessage(MessageFlushUpdates* message);
	virtual void handleMessage(MessageFocusIn* message);
	virtual void handleMessage(MessageFocusOut* message);
	virtual void handleMessage(MessageScrollCode* message);
	virtual void handleMessage(MessageShowErrors* message);
	virtual void handleMessage(MessageShowScope* message);

	CodeView* getView();
	void showContents(MessageBase* message);

	std::vector<CodeView::CodeSnippetParams> getSnippetsForActiveTokenLocations(
		const TokenLocationCollection* collection, Id declarationId) const;
	std::vector<CodeView::CodeSnippetParams> getSnippetsForActiveTokenLocationsInFile(
		std::shared_ptr<TokenLocationFile>) const;
	std::vector<CodeView::CodeSnippetParams> getSnippetsForFile(std::shared_ptr<TokenLocationFile> file) const;
	std::shared_ptr<SnippetMerger> buildMergerHierarchy(
		TokenLocation* location, SnippetMerger& fileScopedMerger, std::map<int, std::shared_ptr<SnippetMerger>>& mergers) const;

	std::vector<CodeView::CodeSnippetParams> getSnippetsForErrorLocations(std::vector<std::string>* errorMessages) const;

	std::vector<std::string> getProjectDescription(TokenLocationFile* locationFile) const;

	StorageAccess* m_storageAccess;
};

#endif // CODE_CONTROLLER_H
