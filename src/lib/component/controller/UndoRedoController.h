#ifndef UNDO_REDO_CONTROLLER_H
#define UNDO_REDO_CONTROLLER_H

#include <list>

#include "MessageActivateErrors.h"
#include "MessageActivateFullTextSearch.h"
#include "MessageActivateLegend.h"
#include "MessageActivateLocalSymbols.h"
#include "MessageActivateOverview.h"
#include "MessageActivateTokens.h"
#include "MessageActivateTrail.h"
#include "MessageActivateTrailEdge.h"
#include "MessageBase.h"
#include "MessageChangeFileView.h"
#include "MessageCodeShowDefinition.h"
#include "MessageDeactivateEdge.h"
#include "MessageGraphNodeBundleSplit.h"
#include "MessageGraphNodeExpand.h"
#include "MessageGraphNodeHide.h"
#include "MessageGraphNodeMove.h"
#include "MessageHistoryRedo.h"
#include "MessageHistoryToPosition.h"
#include "MessageHistoryUndo.h"
#include "MessageIndexingFinished.h"
#include "MessageListener.h"
#include "MessageRefreshUIState.h"
#include "MessageScrollCode.h"
#include "MessageScrollGraph.h"
#include "MessageShowError.h"
#include "MessageShowReference.h"
#include "MessageShowScope.h"

#include "Controller.h"

class StorageAccess;
class UndoRedoView;

class UndoRedoController
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
	, public MessageListener<MessageCodeShowDefinition>
	, public MessageListener<MessageDeactivateEdge>
	, public MessageListener<MessageGraphNodeBundleSplit>
	, public MessageListener<MessageGraphNodeExpand>
	, public MessageListener<MessageGraphNodeHide>
	, public MessageListener<MessageGraphNodeMove>
	, public MessageListener<MessageHistoryRedo>
	, public MessageListener<MessageHistoryToPosition>
	, public MessageListener<MessageHistoryUndo>
	, public MessageListener<MessageIndexingFinished>
	, public MessageListener<MessageRefreshUIState>
	, public MessageListener<MessageScrollCode>
	, public MessageListener<MessageScrollGraph>
	, public MessageListener<MessageShowError>
	, public MessageListener<MessageShowReference>
	, public MessageListener<MessageShowScope>
{
public:
	UndoRedoController(StorageAccess* storageAccess);
	virtual ~UndoRedoController() = default;

	Id getSchedulerId() const override;

	UndoRedoView* getView();

	void clear() override;

private:
	struct Command
	{
		enum Order
		{
			ORDER_ACTIVATE,
			ORDER_ADAPT,
			ORDER_VIEW
		};

		Command(std::shared_ptr<MessageBase> message, Order order, bool replayLastOnly = false);

		std::shared_ptr<MessageBase> message;
		Order order;
		bool replayLastOnly;
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
	void handleMessage(MessageCodeShowDefinition* message) override;
	void handleMessage(MessageDeactivateEdge* message) override;
	void handleMessage(MessageGraphNodeBundleSplit* message) override;
	void handleMessage(MessageGraphNodeExpand* message) override;
	void handleMessage(MessageGraphNodeHide* message) override;
	void handleMessage(MessageGraphNodeMove* message) override;
	void handleMessage(MessageHistoryRedo* message) override;
	void handleMessage(MessageHistoryToPosition* message) override;
	void handleMessage(MessageHistoryUndo* message) override;
	void handleMessage(MessageIndexingFinished* message) override;
	void handleMessage(MessageRefreshUIState* message) override;
	void handleMessage(MessageScrollCode* message) override;
	void handleMessage(MessageScrollGraph* message) override;
	void handleMessage(MessageShowError* message) override;
	void handleMessage(MessageShowReference* message) override;
	void handleMessage(MessageShowScope* message) override;

	void replayCommands();
	void replayCommands(std::list<Command>::iterator it);
	void replayCommand(std::list<Command>::iterator it);

	void processCommand(Command command);

	bool sameMessageTypeAsLast(MessageBase* message) const;
	MessageBase* lastMessage() const;

	void updateHistoryMenu(std::shared_ptr<MessageBase> message);
	void updateHistory();

	void dump() const;

	StorageAccess* m_storageAccess;

	std::list<Command> m_list;
	std::list<Command>::iterator m_iterator;

	std::vector<std::shared_ptr<MessageBase>> m_history;
	size_t m_historyOffset;
};

#endif	  // UNDO_REDO_CONTROLLER_H
