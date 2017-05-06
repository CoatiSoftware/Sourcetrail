#ifndef UNDO_REDO_CONTROLLER_H
#define UNDO_REDO_CONTROLLER_H

#include <list>

#include "utility/messaging/MessageBase.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateAll.h"
#include "utility/messaging/type/MessageActivateLocalSymbols.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageActivateTrail.h"
#include "utility/messaging/type/MessageActivateTrailEdge.h"
#include "utility/messaging/type/MessageChangeFileView.h"
#include "utility/messaging/type/MessageDeactivateEdge.h"
#include "utility/messaging/type/MessageFinishedParsing.h"
#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"
#include "utility/messaging/type/MessageGraphNodeExpand.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageScrollCode.h"
#include "utility/messaging/type/MessageScrollGraph.h"
#include "utility/messaging/type/MessageSearchFullText.h"
#include "utility/messaging/type/MessageShowErrors.h"
#include "utility/messaging/type/MessageShowReference.h"
#include "utility/messaging/type/MessageShowScope.h"
#include "utility/messaging/type/MessageUndo.h"

#include "component/controller/Controller.h"

class StorageAccess;
class UndoRedoView;

class UndoRedoController
	: public Controller
	, public MessageListener<MessageActivateAll>
	, public MessageListener<MessageActivateLocalSymbols>
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageActivateTrail>
	, public MessageListener<MessageActivateTrailEdge>
	, public MessageListener<MessageChangeFileView>
	, public MessageListener<MessageDeactivateEdge>
	, public MessageListener<MessageFinishedParsing>
	, public MessageListener<MessageGraphNodeBundleSplit>
	, public MessageListener<MessageGraphNodeExpand>
	, public MessageListener<MessageGraphNodeMove>
	, public MessageListener<MessageRedo>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageScrollCode>
	, public MessageListener<MessageScrollGraph>
	, public MessageListener<MessageSearchFullText>
	, public MessageListener<MessageShowErrors>
	, public MessageListener<MessageShowReference>
	, public MessageListener<MessageShowScope>
	, public MessageListener<MessageUndo>
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
	virtual void handleMessage(MessageActivateLocalSymbols* message);
	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageActivateTrail* message);
	virtual void handleMessage(MessageActivateTrailEdge* message);
	virtual void handleMessage(MessageChangeFileView* message);
	virtual void handleMessage(MessageDeactivateEdge* message);
	virtual void handleMessage(MessageFinishedParsing* message);
	virtual void handleMessage(MessageGraphNodeBundleSplit* message);
	virtual void handleMessage(MessageGraphNodeExpand* message);
	virtual void handleMessage(MessageGraphNodeMove* message);
	virtual void handleMessage(MessageRedo* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageScrollCode* message);
	virtual void handleMessage(MessageScrollGraph* message);
	virtual void handleMessage(MessageSearchFullText* message);
	virtual void handleMessage(MessageShowErrors* message);
	virtual void handleMessage(MessageShowReference* message);
	virtual void handleMessage(MessageShowScope* message);
	virtual void handleMessage(MessageUndo* message);

	void replayCommands();
	void replayCommands(std::list<Command>::iterator it);
	void replayCommand(std::list<Command>::iterator it);

	void processCommand(Command command);

	bool sameMessageTypeAsLast(MessageBase* message) const;
	MessageBase* lastMessage() const;

	void dump() const;

	StorageAccess* m_storageAccess;

	std::list<Command> m_list;
	std::list<Command>::iterator m_iterator;
};

#endif // UNDO_REDO_CONTROLLER_H
