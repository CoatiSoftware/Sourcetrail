#ifndef CODE_CONTROLLER_H
#define CODE_CONTROLLER_H

#include <map>
#include <string>

#include "FilePath.h"
#include "MessageListener.h"
#include "MessageActivateLegend.h"
#include "MessageCodeShowDefinition.h"
#include "MessageActivateErrors.h"
#include "MessageErrorCountClear.h"
#include "MessageShowError.h"
#include "MessageActivateAll.h"
#include "MessageActivateFullTextSearch.h"
#include "MessageActivateLocalSymbols.h"
#include "MessageActivateTokens.h"
#include "MessageActivateTrailEdge.h"
#include "MessageChangeFileView.h"
#include "MessageDeactivateEdge.h"
#include "MessageFlushUpdates.h"
#include "MessageFocusIn.h"
#include "MessageFocusOut.h"
#include "MessageScrollCode.h"
#include "MessageScrollToLine.h"
#include "MessageShowScope.h"
#include "types.h"

#include "SnippetMerger.h"
#include "Controller.h"
#include "CodeView.h"

class StorageAccess;
class SourceLocation;
class SourceLocationCollection;
class SourceLocationFile;

class CodeController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateErrors>
	, public MessageListener<MessageActivateFullTextSearch>
	, public MessageListener<MessageActivateLegend>
	, public MessageListener<MessageActivateLocalSymbols>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageActivateTrailEdge>
	, public MessageListener<MessageChangeFileView>
	, public MessageListener<MessageCodeShowDefinition>
	, public MessageListener<MessageDeactivateEdge>
	, public MessageListener<MessageErrorCountClear>
	, public MessageListener<MessageFlushUpdates>
	, public MessageListener<MessageFocusIn>
	, public MessageListener<MessageFocusOut>
	, public MessageListener<MessageScrollCode>
	, public MessageListener<MessageScrollToLine>
	, public MessageListener<MessageShowError>
	, public MessageListener<MessageShowScope>
{
public:
	CodeController(StorageAccess* storageAccess);
	virtual ~CodeController() = default;

	Id getSchedulerId() const override;

private:
	static const uint s_lineRadius;

	void handleMessage(MessageActivateAll* message) override;
	void handleMessage(MessageActivateErrors* message) override;
	void handleMessage(MessageActivateFullTextSearch* message) override;
	void handleMessage(MessageActivateLegend* message) override;
	void handleMessage(MessageActivateLocalSymbols* message) override;
	void handleMessage(MessageActivateTokens* message) override;
	void handleMessage(MessageActivateTrailEdge* message) override;
	void handleMessage(MessageChangeFileView* message) override;
	void handleMessage(MessageCodeShowDefinition* message) override;
	void handleMessage(MessageDeactivateEdge* message) override;
	void handleMessage(MessageErrorCountClear* message) override;
	void handleMessage(MessageFlushUpdates* message) override;
	void handleMessage(MessageFocusIn* message) override;
	void handleMessage(MessageFocusOut* message) override;
	void handleMessage(MessageScrollCode* message) override;
	void handleMessage(MessageScrollToLine* message) override;
	void handleMessage(MessageShowError* message) override;
	void handleMessage(MessageShowScope* message) override;

	CodeView* getView() const;

	void clear() override;

	std::vector<CodeSnippetParams> getSnippetsForFileWithState(const FilePath& filePath, CodeView::FileState state) const;
	std::vector<CodeSnippetParams> getSnippetsForActiveSourceLocations(
		const SourceLocationCollection* collection, Id declarationId) const;
	std::vector<CodeSnippetParams> getSnippetsForCollection(std::shared_ptr<SourceLocationCollection> collection) const;
	std::vector<CodeSnippetParams> getSnippetsForFile(std::shared_ptr<SourceLocationFile> file) const;

	std::shared_ptr<SnippetMerger> buildMergerHierarchy(
		const SourceLocation* location, const SourceLocationFile* scopeLocations, SnippetMerger& fileScopedMerger,
		std::map<int, std::shared_ptr<SnippetMerger>>& mergers) const;
	const SourceLocation* getSourceLocationOfParentScope(
		size_t lineNumber, const SourceLocationFile* scopeLocations) const;

	std::vector<std::string> getProjectDescription(SourceLocationFile* locationFile) const;

	void expandVisibleSnippets(std::vector<CodeSnippetParams>* snippets, bool useSingleFileCache) const;
	void addAllSourceLocations(std::vector<CodeSnippetParams>* snippets) const;
	void addModificationTimes(std::vector<CodeSnippetParams>* snippets) const;

	void saveOrRestoreViewMode(MessageBase* message);

	void showCodeSnippets(
		std::vector<CodeSnippetParams> snippets, const CodeView::CodeParams params, bool addSourceLocations = true);

	StorageAccess* m_storageAccess;
	mutable std::shared_ptr<SourceLocationCollection> m_collection;

	std::map<Id, bool> m_messageIdToViewModeMap;
};

#endif // CODE_CONTROLLER_H
