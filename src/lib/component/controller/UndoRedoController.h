#ifndef UNDO_REDO_CONTROLLER_H
#define UNDO_REDO_CONTROLLER_H

#include <deque>

#include "utility/messaging/MessageBase.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateEdge.h"
#include "utility/messaging/type/MessageActivateFile.h"
#include "utility/messaging/type/MessageActivateNodes.h"
#include "utility/messaging/type/MessageGraphNodeBundleSplit.h"
#include "utility/messaging/type/MessageGraphNodeExpand.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"
#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageSearch.h"
#include "utility/messaging/type/MessageShowFile.h"
#include "utility/messaging/type/MessageShowScope.h"
#include "utility/messaging/type/MessageUndo.h"

#include "component/controller/Controller.h"

class UndoRedoView;

class UndoRedoController
	: public Controller
	, public MessageListener<MessageActivateEdge>
	, public MessageListener<MessageActivateFile>
	, public MessageListener<MessageActivateNodes>
	, public MessageListener<MessageGraphNodeBundleSplit>
	, public MessageListener<MessageGraphNodeExpand>
	, public MessageListener<MessageGraphNodeMove>
	, public MessageListener<MessageLoadProject>
	, public MessageListener<MessageRedo>
	, public MessageListener<MessageRefresh>
	, public MessageListener<MessageSearch>
	, public MessageListener<MessageShowFile>
	, public MessageListener<MessageShowScope>
	, public MessageListener<MessageUndo>
{
public:
	UndoRedoController();
	virtual ~UndoRedoController();

	UndoRedoView* getView();

private:
	struct Command
	{
		Command(std::shared_ptr<MessageBase> message, size_t order);

		std::shared_ptr<MessageBase> message;
		size_t order;
	};

	virtual void handleMessage(MessageActivateEdge* message);
	virtual void handleMessage(MessageActivateFile* message);
	virtual void handleMessage(MessageActivateNodes* message);
	virtual void handleMessage(MessageGraphNodeBundleSplit* message);
	virtual void handleMessage(MessageGraphNodeExpand* message);
	virtual void handleMessage(MessageGraphNodeMove* message);
	virtual void handleMessage(MessageLoadProject* message);
	virtual void handleMessage(MessageRedo* message);
	virtual void handleMessage(MessageRefresh* message);
	virtual void handleMessage(MessageSearch* message);
	virtual void handleMessage(MessageShowFile* message);
	virtual void handleMessage(MessageShowScope* message);
	virtual void handleMessage(MessageUndo* message);

	void replayCommands(bool removeLast);

	void processCommand(const Command& command);
	void processNormalCommand(const Command& command);
	void processRedoCommand(const Command& command);
	void processUndoCommand(const Command& command);

	void clear();

	Command m_lastCommand;

	std::deque<Command> m_undo;
	std::deque<Command> m_redo;
};

#endif // UNDO_REDO_CONTROLLER_H
