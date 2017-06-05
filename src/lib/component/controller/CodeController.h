#ifndef CODE_CONTROLLER_H
#define CODE_CONTROLLER_H

#include <map>
#include <string>

#include "utility/file/FilePath.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateLocalSymbols.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageActivateTrailEdge.h"
#include "utility/messaging/type/MessageChangeFileView.h"
#include "utility/messaging/type/MessageClearErrorCount.h"
#include "utility/messaging/type/MessageDeactivateEdge.h"
#include "utility/messaging/type/MessageFlushUpdates.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageScrollCode.h"
#include "utility/messaging/type/MessageScrollToLine.h"
#include "utility/messaging/type/MessageSearchFullText.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageShowScope.h"
#include "utility/types.h"

#include "component/controller/helper/SnippetMerger.h"
#include "component/controller/Controller.h"
#include "component/view/CodeView.h"

class StorageAccess;
class SourceLocation;
class SourceLocationCollection;
class SourceLocationFile;

class CodeController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateLocalSymbols>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageActivateTrailEdge>
	, public MessageListener<MessageChangeFileView>
	, public MessageListener<MessageClearErrorCount>
	, public MessageListener<MessageDeactivateEdge>
	, public MessageListener<MessageFlushUpdates>
	, public MessageListener<MessageFocusIn>
	, public MessageListener<MessageFocusOut>
	, public MessageListener<MessageScrollCode>
	, public MessageListener<MessageScrollToLine>
	, public MessageListener<MessageSearchFullText>
	, public MessageListener<MessageShowErrors>
	, public MessageListener<MessageShowScope>
{
public:
	CodeController(StorageAccess* storageAccess);
	~CodeController();

private:
	static const uint s_lineRadius;

	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateLocalSymbols* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageActivateTrailEdge* message);
	virtual void handleMessage(MessageChangeFileView* message);
	virtual void handleMessage(MessageClearErrorCount* message);
	virtual void handleMessage(MessageDeactivateEdge* message);
	virtual void handleMessage(MessageFlushUpdates* message);
	virtual void handleMessage(MessageFocusIn* message);
	virtual void handleMessage(MessageFocusOut* message);
	virtual void handleMessage(MessageScrollCode* message);
	virtual void handleMessage(MessageScrollToLine* message);
	virtual void handleMessage(MessageSearchFullText* message);
	virtual void handleMessage(MessageShowErrors* message);
	virtual void handleMessage(MessageShowScope* message);

	CodeView* getView() const;

	virtual void clear();

	void expandVisibleSnippets(std::vector<CodeSnippetParams>* snippets) const;

	std::vector<CodeSnippetParams> getSnippetsForFileWithState(
		const FilePath& filePath, CodeView::FileState state, bool addSourceLocations) const;

	std::vector<CodeSnippetParams> getSnippetsForActiveSourceLocations(
		const SourceLocationCollection* collection, Id declarationId) const;
	std::vector<CodeSnippetParams> getSnippetsForCollection(
		std::shared_ptr<SourceLocationCollection> collection, bool addSourceLocations = false) const;
	std::vector<CodeSnippetParams> getSnippetsForFile(
		std::shared_ptr<SourceLocationFile> file, bool addSourceLocations = false) const;

	std::shared_ptr<SnippetMerger> buildMergerHierarchy(
		SourceLocation* location, std::shared_ptr<SourceLocationFile> context, SnippetMerger& fileScopedMerger,
		std::map<int, std::shared_ptr<SnippetMerger>>& mergers) const;
	std::shared_ptr<SourceLocationFile> getSourceLocationOfParentScope(
		const SourceLocation* location, std::shared_ptr<SourceLocationFile> context) const;

	std::vector<std::string> getProjectDescription(SourceLocationFile* locationFile) const;

	void addModificationTimes(std::vector<CodeSnippetParams>& snippets) const;
	void addActiveSourceLocations(std::shared_ptr<SourceLocationFile> locationFile) const;

	StorageAccess* m_storageAccess;
	mutable std::shared_ptr<SourceLocationCollection> m_collection;
};

#endif // CODE_CONTROLLER_H
