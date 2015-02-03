#include "component/controller/UndoRedoController.h"

#include <iostream>

#include "component/view/UndoRedoView.h"
#include "utility/logging/logging.h"

UndoRedoController::UndoRedoController()
	: MessageListener<MessageActivateTokens>(true)
	, MessageListener<MessageGraphNodeExpand>(true)
	, MessageListener<MessageGraphNodeMove>(true)
	, m_lastCommand(nullptr)
{
}

UndoRedoController::~UndoRedoController()
{
	for(MessageBase* m : m_redo)
	{
		delete m;
	}
	for(MessageBase* m : m_undo)
	{
		delete m;
	}
}

UndoRedoView* UndoRedoController::getView()
{
	return Controller::getView<UndoRedoView>();
}

void UndoRedoController::handleMessage(MessageActivateTokens* message)
{
    MessageActivateTokens* m = new MessageActivateTokens(message->tokenIds);
    m->isAggregation = message->isAggregation;
    m->isEdge = message->isEdge;
    m->UndoRedoType = message->UndoRedoType;

    processMessage(m);
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
        MessageBase* m = m_redo.back();
        m_redo.pop_back();
        m->UndoRedoType = MessageBase::UndoType_Redo;
        m->dispatch();
        delete m;
    }
}

void UndoRedoController::handleMessage(MessageUndo* message)
{
    if(!m_undo.empty())
    {
    	MessageBase* m = m_undo.back();
        m_undo.pop_back();
    	m->UndoRedoType = MessageBase::UndoType_Undo;
    	m->dispatch();
        delete m;
    }
}

void UndoRedoController::processMessage(MessageBase* message)
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


void UndoRedoController::processNormalMessage(MessageBase* message)
{
    if(m_lastCommand != nullptr)
    {
        m_undo.push_back(m_lastCommand);
        getView()->setUndoButtonEnabled(true);
    }
    m_lastCommand = message;
    while(!m_redo.empty())
    {
        MessageBase* m = m_redo.back();
        m_redo.pop_back();
        delete m;
    }
    getView()->setRedoButtonEnabled(false);
}

void UndoRedoController::processRedoMessage(MessageBase* message)
{
    m_undo.push_back(m_lastCommand);
    getView()->setUndoButtonEnabled(true);
    m_lastCommand = message;
    if(m_redo.empty())
    {
        getView()->setRedoButtonEnabled(false);
    }
}

void UndoRedoController::processUndoMessage(MessageBase* message)
{
    m_redo.push_back(m_lastCommand);
    getView()->setRedoButtonEnabled(true);
    m_lastCommand = message;
    if(m_undo.empty())
    {
        getView()->setUndoButtonEnabled(false);
    }
}

