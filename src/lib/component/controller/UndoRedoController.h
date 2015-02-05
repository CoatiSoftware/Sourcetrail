#ifndef UNDO_REDO_CONTROLLER_H
#define UNDO_REDO_CONTROLLER_H

#include <deque>
#include <memory>
#include <stack>
#include <string>

#include "component/controller/Controller.h"

#include "utility/messaging/MessageBase.h"
#include "utility/messaging/MessageListener.h"
#include "utility/messaging/type/MessageActivateTokens.h"
#include "utility/messaging/type/MessageGraphNodeExpand.h"
#include "utility/messaging/type/MessageGraphNodeMove.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/messaging/type/MessageUndo.h"

class UndoRedoView;

class UndoRedoController
	: public Controller
	, public MessageListener<MessageActivateTokens>
	, public MessageListener<MessageGraphNodeExpand>
	, public MessageListener<MessageGraphNodeMove>
	, public MessageListener<MessageRedo>
	, public MessageListener<MessageUndo>
{
public:
	UndoRedoController(void);
	virtual ~UndoRedoController(void);
	UndoRedoView* getView();
private:
    enum UndoRedoType
    {
        UndoRedoType_Normal,
        UndoRedoType_Undo,
        UndoRedoType_Redo
    };

	virtual void handleMessage(MessageActivateTokens* message);
	virtual void handleMessage(MessageGraphNodeExpand* message);
	virtual void handleMessage(MessageGraphNodeMove* message);
	virtual void handleMessage(MessageRedo* message);
	virtual void handleMessage(MessageUndo* message);

	void processMessage(std::shared_ptr<MessageBase> message);
	void processNormalMessage(std::shared_ptr<MessageBase> message);
	void processRedoMessage(std::shared_ptr<MessageBase> message);
	void processUndoMessage(std::shared_ptr<MessageBase> message);

	std::shared_ptr<MessageBase> m_lastCommand;

	std::deque<std::shared_ptr<MessageBase>> m_undo;
	std::deque<std::shared_ptr<MessageBase>> m_redo;
};

#endif // UNDO_REDO_CONTROLLER_H
