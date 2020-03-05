#ifndef CODE_CONTROLLER_H
#define CODE_CONTROLLER_H

#include <map>
#include <string>

#include "FilePath.h"
#include "LocationType.h"
#include "MessageActivateErrors.h"
#include "MessageActivateFullTextSearch.h"
#include "MessageActivateLegend.h"
#include "MessageActivateLocalSymbols.h"
#include "MessageActivateOverview.h"
#include "MessageActivateTokens.h"
#include "MessageActivateTrail.h"
#include "MessageActivateTrailEdge.h"
#include "MessageChangeFileView.h"
#include "MessageCodeReference.h"
#include "MessageCodeShowDefinition.h"
#include "MessageDeactivateEdge.h"
#include "MessageErrorCountClear.h"
#include "MessageFocusChanged.h"
#include "MessageFlushUpdates.h"
#include "MessageFocusIn.h"
#include "MessageFocusOut.h"
#include "MessageListener.h"
#include "MessageScrollCode.h"
#include "MessageScrollToLine.h"
#include "MessageShowError.h"
#include "MessageShowReference.h"
#include "MessageShowScope.h"
#include "MessageToNextCodeReference.h"
#include "types.h"

#include "CodeView.h"
#include "Controller.h"
#include "SnippetMerger.h"

class StorageAccess;
class SourceLocation;
class SourceLocationCollection;
class SourceLocationFile;

class CodeController
	: public Controller
	, public MessageListener<MessageActivateErrors>
	, public MessageListener<MessageActivateFullTextSearch>
	, public MessageListener<MessageActivateLegend>
	, public MessageListener<MessageActivateLocalSymbols>
	, public MessageListener<MessageActivateOverview>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageActivateTrail>
	, public MessageListener<MessageActivateTrailEdge>
	, public MessageListener<MessageChangeFileView>
	, public MessageListener<MessageCodeReference>
	, public MessageListener<MessageCodeShowDefinition>
	, public MessageListener<MessageDeactivateEdge>
	, public MessageListener<MessageErrorCountClear>
	, public MessageListener<MessageFocusChanged>
	, public MessageListener<MessageFlushUpdates>
	, public MessageListener<MessageFocusIn>
	, public MessageListener<MessageFocusOut>
	, public MessageListener<MessageScrollCode>
	, public MessageListener<MessageScrollToLine>
	, public MessageListener<MessageShowError>
	, public MessageListener<MessageShowReference>
	, public MessageListener<MessageShowScope>
	, public MessageListener<MessageToNextCodeReference>
{
public:
	CodeController(StorageAccess* storageAccess);
	virtual ~CodeController() = default;

	Id getSchedulerId() const override;

private:
	struct Reference
	{
		FilePath filePath;
		Id tokenId = 0;
		Id locationId = 0;
		Id scopeLocationId = 0;
		LocationType locationType = LOCATION_TOKEN;
		size_t lineNumber = 0;
		size_t columnNumber = 0;
	};

	void handleMessage(MessageActivateErrors* message) override;
	void handleMessage(MessageActivateFullTextSearch* message) override;
	void handleMessage(MessageActivateLegend* message) override;
	void handleMessage(MessageActivateLocalSymbols* message) override;
	void handleMessage(MessageActivateOverview* message) override;
	void handleMessage(MessageActivateTokens* message) override;
	void handleMessage(MessageActivateTrail* message) override;
	void handleMessage(MessageActivateTrailEdge* message) override;
	void handleMessage(MessageChangeFileView* message) override;
	void handleMessage(MessageCodeReference* message) override;
	void handleMessage(MessageCodeShowDefinition* message) override;
	void handleMessage(MessageDeactivateEdge* message) override;
	void handleMessage(MessageErrorCountClear* message) override;
	void handleMessage(MessageFocusChanged* message) override;
	void handleMessage(MessageFlushUpdates* message) override;
	void handleMessage(MessageFocusIn* message) override;
	void handleMessage(MessageFocusOut* message) override;
	void handleMessage(MessageScrollCode* message) override;
	void handleMessage(MessageScrollToLine* message) override;
	void handleMessage(MessageShowError* message) override;
	void handleMessage(MessageShowReference* message) override;
	void handleMessage(MessageShowScope* message) override;
	void handleMessage(MessageToNextCodeReference* message) override;

	CodeView* getView() const;

	void clear() override;

	std::vector<CodeFileParams> getFilesForActiveSourceLocations(
		const SourceLocationCollection* collection, Id declarationId) const;
	std::vector<CodeFileParams> getFilesForCollection(
		std::shared_ptr<SourceLocationCollection> collection) const;
	CodeSnippetParams getSnippetParamsForWholeFile(
		std::shared_ptr<SourceLocationFile> locationFile, bool useSingleFileCache) const;
	std::vector<CodeSnippetParams> getSnippetsForFile(
		std::shared_ptr<SourceLocationFile> activeSourceLocations) const;

	std::shared_ptr<SnippetMerger> buildMergerHierarchy(
		const SourceLocation* location,
		const SourceLocationFile* scopeLocations,
		SnippetMerger& fileScopedMerger,
		std::map<int, std::shared_ptr<SnippetMerger>>& mergers) const;
	const SourceLocation* getSourceLocationOfParentScope(
		size_t lineNumber, const SourceLocationFile* scopeLocations) const;

	std::vector<std::string> getProjectDescription(SourceLocationFile* locationFile) const;

	void clearReferences();
	void createReferences();

	void clearLocalReferences();
	void createLocalReferences(const std::set<Id>& localSymbolIds);

	void iterateReference(bool next);
	void iterateLocalReference(bool next, bool updateView);

	void showCurrentReference();
	void showCurrentLocalReference(bool updateView);

	std::pair<int, int> findClosestReferenceIndex(
		const std::vector<Reference>& references,
		const FilePath& currentFilePath,
		size_t currentLineNumber,
		size_t currentColumnNumber,
		bool next) const;

	void expandVisibleFiles(bool useSingleFileCache);
	CodeFileParams* addSourceLocations(std::shared_ptr<SourceLocationFile> locationFile);
	void setFileState(
		const FilePath& filePath, MessageChangeFileView::FileState state, bool useSingleFileCache);
	void setFileState(
		CodeFileParams& file, MessageChangeFileView::FileState state, bool useSingleFileCache);
	bool addAllSourceLocations();
	void addModificationTimes();

	CodeScrollParams firstReferenceScrollParams() const;
	CodeScrollParams definitionReferenceScrollParams(const std::vector<Id>& activeTokenIds) const;
	CodeScrollParams toReferenceScrollParams(const Reference& ref) const;

	void saveOrRestoreViewMode(MessageBase* message);

	void showFirstActiveReference(Id tokenId, bool updateView);
	void showFiles(CodeView::CodeParams params, CodeScrollParams scrollParams, bool updateView);

	StorageAccess* m_storageAccess;

	std::shared_ptr<SourceLocationCollection> m_collection;

	std::vector<CodeFileParams> m_files;
	FilePath m_currentFilePath;

	CodeView::CodeParams m_codeParams;
	CodeScrollParams m_scrollParams;

	std::map<Id, bool> m_messageIdToViewModeMap;

	std::vector<Reference> m_references;
	int m_referenceIndex = -1;

	std::vector<Reference> m_localReferences;
	int m_localReferenceIndex = -1;
};

#endif	  // CODE_CONTROLLER_H
