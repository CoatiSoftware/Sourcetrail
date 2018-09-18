#ifndef UNDO_REDO_CONTROLLER_H
#define UNDO_REDO_CONTROLLER_H

#include <list>

#include "MessageBase.h"
#include "MessageListener.h"
#include "MessageActivateLegend.h"
#include "MessageCodeShowDefinition.h"
#include "MessageActivateErrors.h"
#include "MessageShowError.h"
#include "MessageHistoryRedo.h"
#include "MessageHistoryToPosition.h"
#include "MessageHistoryUndo.h"
#include "MessageIndexingFinished.h"
#include "MessageActivateAll.h"
#include "MessageActivateFullTextSearch.h"
#include "MessageActivateLocalSymbols.h"
#include "MessageActivateTokens.h"
#include "MessageActivateTrail.h"
#include "MessageActivateTrailEdge.h"
#include "MessageChangeFileView.h"
#include "MessageDeactivateEdge.h"
#include "MessageGraphNodeBundleSplit.h"
#include "MessageGraphNodeExpand.h"
#include "MessageGraphNodeHide.h"
#include "MessageGraphNodeMove.h"
#include "MessageRefresh.h"
#include "MessageScrollCode.h"
#include "MessageScrollGraph.h"
#include "MessageShowReference.h"
#include "MessageShowScope.h"

#include "Controller.h"

class StorageAccess;
class UndoRedoView;

class UndoRedoController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateErrors>
	, public MessageListener<MessageActivateFullTextSearch>
	, public MessageListener<MessageActivateLegend>
	, public MessageListener<MessageActivateLocalSymbols>
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
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageScrollCode>
	, public MessageListener<MessageScrollGraph>
	, public MessageListener<MessageShowError>
	, public MessageListener<MessageShowReference>
	, public MessageListener<MessageShowScope>
{
public:
	UndoRedoController(StorageAccess* storageAccess);
	virtual ~UndoRedoController();

	UndoRedoView* getView();

	virtual void clear();

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

	virtual void handleMessage(MessageActivateAll* message);
	virtual void handleMessage(MessageActivateErrors* message);
	virtual void handleMessage(MessageActivateFullTextSearch* message);
	virtual void handleMessage(MessageActivateLegend* message);
	virtual void handleMessage(MessageActivateLocalSymbols* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageActivateTrail* message);
	virtual void handleMessage(MessageActivateTrailEdge* message);
	virtual void handleMessage(MessageChangeFileView* message);
	virtual void handleMessage(MessageCodeShowDefinition* message);
	virtual void handleMessage(MessageDeactivateEdge* message);
	virtual void handleMessage(MessageGraphNodeBundleSplit* message);
	virtual void handleMessage(MessageGraphNodeExpand* message);
	virtual void handleMessage(MessageGraphNodeHide* message);
	virtual void handleMessage(MessageGraphNodeMove* message);
	virtual void handleMessage(MessageHistoryRedo* message);
	virtual void handleMessage(MessageHistoryToPosition* message);
	virtual void handleMessage(MessageHistoryUndo* message);
	virtual void handleMessage(MessageIndexingFinished* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageScrollCode* message);
	virtual void handleMessage(MessageScrollGraph* message);
	virtual void handleMessage(MessageShowError* message);
	virtual void handleMessage(MessageShowReference* message);
	virtual void handleMessage(MessageShowScope* message);

	void replayCommands();
	void replayCommands(std::list<Command>::iterator it);
	void replayCommand(std::list<Command>::iterator it);

	void processCommand(Command command);

	bool sameMessageTypeAsLast(MessageBase* message) const;
	MessageBase* lastMessage() const;

	void updateHistory();

	void dump() const;

	StorageAccess* m_storageAccess;

	std::list<Command> m_list;
	std::list<Command>::iterator m_iterator;

	size_t m_historyOffset;
};

#endif // UNDO_REDO_CONTROLLER_H
