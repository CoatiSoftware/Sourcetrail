#include "component/controller/UndoRedoController.h"

#include "utility/logging/logging.h"

#include "component/view/UndoRedoView.h"

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

void UndoRedoController::handleMessage(MessageActivateEdge* message)
{
	if (m_lastCommand && m_lastCommand->getType() == message->getType() &&
		static_cast<MessageActivateEdge*>(m_lastCommand.get())->name == message->name)
	{
		return;
	}

	processMessage(std::make_shared<MessageActivateEdge>(*message));
}

void UndoRedoController::handleMessage(MessageActivateFile* message)
{
	if (m_lastCommand && m_lastCommand->getType() == message->getType() &&
		static_cast<MessageActivateFile*>(m_lastCommand.get())->filePath == message->filePath)
	{
		return;
	}

	processMessage(std::make_shared<MessageActivateFile>(*message));
}

void UndoRedoController::handleMessage(MessageActivateNode* message)
{
	if (m_lastCommand && m_lastCommand->getType() == message->getType() &&
		static_cast<MessageActivateNode*>(m_lastCommand.get())->name == message->name)
	{
		return;
	}

	processMessage(std::make_shared<MessageActivateNode>(*message));
}

void UndoRedoController::handleMessage(MessageGraphNodeExpand* message)
{
}

void UndoRedoController::handleMessage(MessageGraphNodeMove* message)
{
}

void UndoRedoController::handleMessage(MessageLoadProject* message)
{
	clear();
}

void UndoRedoController::handleMessage(MessageLoadSource* message)
{
	clear();
}

void UndoRedoController::handleMessage(MessageRedo* message)
{
	if (!m_redo.empty())
	{
		std::shared_ptr<MessageBase> m = m_redo.back();
		m_redo.pop_back();
		m->undoRedoType = MessageBase::UndoType_Redo;
		m->dispatch();
	}
}

void UndoRedoController::handleMessage(MessageSearch* message)
{
	if (m_lastCommand && m_lastCommand->getType() == message->getType() &&
		static_cast<MessageSearch*>(m_lastCommand.get())->getQuery() == message->getQuery())
	{
		return;
	}

	processMessage(std::make_shared<MessageSearch>(*message));
}

void UndoRedoController::handleMessage(MessageUndo* message)
{
	if (!m_undo.empty())
	{
		std::shared_ptr<MessageBase> m = m_undo.back();
		m_undo.pop_back();
		m->undoRedoType = MessageBase::UndoType_Undo;
		m->dispatch();
	}
}

void UndoRedoController::processMessage(std::shared_ptr<MessageBase> message)
{
	switch (message->undoRedoType)
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
	if (m_lastCommand)
	{
		m_undo.push_back(m_lastCommand);
		getView()->setUndoButtonEnabled(true);
	}

	m_lastCommand = message;

	m_redo.clear();
	getView()->setRedoButtonEnabled(false);
}

void UndoRedoController::processRedoMessage(std::shared_ptr<MessageBase> message)
{
	m_undo.push_back(m_lastCommand);
	m_lastCommand = message;

	getView()->setUndoButtonEnabled(true);

	if (m_redo.empty())
	{
		getView()->setRedoButtonEnabled(false);
	}
}

void UndoRedoController::processUndoMessage(std::shared_ptr<MessageBase> message)
{
	m_redo.push_back(m_lastCommand);
	m_lastCommand = message;

	getView()->setRedoButtonEnabled(true);

	if (m_undo.empty())
	{
		getView()->setUndoButtonEnabled(false);
	}
}

void UndoRedoController::clear()
{
	m_lastCommand = nullptr;

	m_undo.clear();
	m_redo.clear();

	getView()->setUndoButtonEnabled(false);
	getView()->setRedoButtonEnabled(false);
}
