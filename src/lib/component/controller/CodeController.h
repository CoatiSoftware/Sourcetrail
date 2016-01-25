#ifndef CODE_CONTROLLER_H
#define CODE_CONTROLLER_H

#include <map>
#include <string>

#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageShowFile.h"
#include "utility/messaging/type/MessageShowScope.h"
#include "utility/messaging/type/MessageShowSnippets.h"
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
	, public MessageListener<MessageFocusIn>
	, public MessageListener<MessageFocusOut>
	, public MessageListener<MessageShowErrors>
	, public MessageListener<MessageShowFile>
	, public MessageListener<MessageShowScope>
	, public MessageListener<MessageShowSnippets>
{
public:
	CodeController(StorageAccess* storageAccess);
	~CodeController();

private:
	static const uint s_lineRadius;

	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageFocusIn* message);
	virtual void handleMessage(MessageFocusOut* message);
	virtual void handleMessage(MessageShowErrors* message);
	virtual void handleMessage(MessageShowFile* message);
	virtual void handleMessage(MessageShowScope* message);
	virtual void handleMessage(MessageShowSnippets* message);

	CodeView* getView();

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
