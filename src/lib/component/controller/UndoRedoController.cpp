#include "component/controller/UndoRedoController.h"

#include "component/view/UndoRedoView.h"
#include "utility/logging/logging.h"

UndoRedoController::UndoRedoController()
	: m_lastCommand(nullptr)
{
}

UndoRedoController::~UndoRedoController()
{
}

UndoRedoView* UndoRedoController::getView()
{
	return Controller::getView<UndoRedoView>();
}

void UndoRedoController::handleMessage(MessageActivateTokens* message)
{
    if(m_lastCommand && m_lastCommand->getType() == "MessageActivateTokens")
    {
        if(static_cast<MessageActivateTokens*>(m_lastCommand.get())->tokenIds == message->tokenIds)
        {
            return;
        }
    }
    std::shared_ptr<MessageActivateTokens> m = std::shared_ptr<MessageActivateTokens>(new MessageActivateTokens(*message));
    std::shared_ptr<MessageBase> msg = m;
    processMessage(msg);
}

void UndoRedoController::handleMessage(MessageGraphNodeExpand* message)
{
    /*MessageGraphNodeExpand* m = new MessageGraphNodeExpand(message->tokenId,message->access);
    m->UndoRedoType = message->UndoRedoType;
    processMessage(m);*/
}

void UndoRedoController::handleMessage(MessageGraphNodeMove* message)
{
    /*MessageGraphNodeMove* m = new MessageGraphNodeMove(message->tokenId,message->position);
    m->UndoRedoType = message->UndoRedoType;
    processMessage(m, true);*/
}

void UndoRedoController::handleMessage(MessageRedo* message)
{
    if(!m_redo.empty())
    {
        std::shared_ptr<MessageBase> m = m_redo.back();
        m_redo.pop_back();
        m->UndoRedoType = MessageBase::UndoType_Redo;
        m->dispatch();
    }
}

void UndoRedoController::handleMessage(MessageUndo* message)
{
    if(!m_undo.empty())
    {
    	std::shared_ptr<MessageBase> m = m_undo.back();
        m_undo.pop_back();
    	m->UndoRedoType = MessageBase::UndoType_Undo;
    	m->dispatch();
    }
}

void UndoRedoController::processMessage(std::shared_ptr<MessageBase> message)
{
    switch(message->UndoRedoType)
    {
        case MessageBase::UndoType_Normal:
            processNormalMessage(message);;
            break;
        case MessageBase::UndoType_Redo:
            processRedoMessage(message);
            break;
        case MessageBase::UndoType_Undo:
            processUndoMessage(message);
            break;
    }
}


void UndoRedoController::processNormalMessage(std::shared_ptr<MessageBase> message)
{
    if(m_lastCommand != nullptr)
    {
        m_undo.push_back(m_lastCommand);
        getView()->setUndoButtonEnabled(true);
    }
    m_lastCommand = message;
    while(!m_redo.empty())
    {
        std::shared_ptr<MessageBase> m = m_redo.back();
        m_redo.pop_back();
    }
    getView()->setRedoButtonEnabled(false);
}

void UndoRedoController::processRedoMessage(std::shared_ptr<MessageBase> message)
{
    m_undo.push_back(m_lastCommand);
    getView()->setUndoButtonEnabled(true);
    m_lastCommand = message;
    if(m_redo.empty())
    {
        getView()->setRedoButtonEnabled(false);
    }
}

void UndoRedoController::processUndoMessage(std::shared_ptr<MessageBase> message)
{
    m_redo.push_back(m_lastCommand);
    getView()->setRedoButtonEnabled(true);
    m_lastCommand = message;
    if(m_undo.empty())
    {
        getView()->setUndoButtonEnabled(false);
    }
}

