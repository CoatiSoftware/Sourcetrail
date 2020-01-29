#include "QtUndoRedo.h"

#include <QHBoxLayout>
#include <QTimer>

#include "MessageHistoryRedo.h"
#include "MessageHistoryUndo.h"
#include "ResourcePaths.h"

#include "QtContextMenu.h"
#include "QtHistoryList.h"
#include "QtSearchBarButton.h"

QtUndoRedo::QtUndoRedo()
	: m_pressed(false), m_historyList(nullptr), m_historyHiddenAt(TimeStamp::now())
{
	setObjectName(QStringLiteral("undo_redo_bar"));

	m_undoButton = new QtSearchBarButton(
		ResourcePaths::getGuiPath().concatenate(L"undoredo_view/images/arrow_left.png"));
	m_undoButton->setObjectName(QStringLiteral("undo_button"));
	m_undoButton->setToolTip(QStringLiteral("back"));
	m_undoButton->setEnabled(false);

	m_historyButton = new QtSearchBarButton(
		ResourcePaths::getGuiPath().concatenate(L"undoredo_view/images/history.png"));
	m_historyButton->setObjectName(QStringLiteral("history_button"));
	m_historyButton->setToolTip(QStringLiteral("history"));
	m_historyButton->setEnabled(false);

	m_redoButton = new QtSearchBarButton(
		ResourcePaths::getGuiPath().concatenate(L"undoredo_view/images/arrow_right.png"));
	m_redoButton->setObjectName(QStringLiteral("redo_button"));
	m_redoButton->setToolTip(QStringLiteral("forward"));
	m_redoButton->setEnabled(false);

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);
	layout->addWidget(m_undoButton);
	layout->addWidget(m_historyButton);
	layout->addWidget(m_redoButton);

	connect(m_undoButton, &QPushButton::pressed, this, &QtUndoRedo::buttonPressed);
	connect(m_redoButton, &QPushButton::pressed, this, &QtUndoRedo::buttonPressed);
	connect(m_historyButton, &QPushButton::pressed, this, &QtUndoRedo::buttonPressed);

	connect(m_undoButton, &QPushButton::released, this, &QtUndoRedo::undoReleased);
	connect(m_redoButton, &QPushButton::released, this, &QtUndoRedo::redoReleased);
	connect(m_historyButton, &QPushButton::released, this, &QtUndoRedo::showHistory);
}

QtUndoRedo::~QtUndoRedo() {}

void QtUndoRedo::setUndoButtonEnabled(bool enabled)
{
	m_undoButton->setEnabled(enabled);
	QtContextMenu::getInstance()->enableUndo(enabled);
}

void QtUndoRedo::setRedoButtonEnabled(bool enabled)
{
	m_redoButton->setEnabled(enabled);
	QtContextMenu::getInstance()->enableRedo(enabled);
}

void QtUndoRedo::updateHistory(const std::vector<SearchMatch>& searchMatches, size_t currentIndex)
{
	m_history = searchMatches;
	m_currentIndex = currentIndex;
	m_historyButton->setEnabled(m_history.size() > 1);
}

void QtUndoRedo::buttonPressed()
{
	if (TimeStamp::now().deltaMS(m_historyHiddenAt) < 250)
	{
		return;
	}

	m_pressed = true;
	QTimer::singleShot(250, this, &QtUndoRedo::showHistory);
	m_historyButton->setAttribute(Qt::WA_UnderMouse, false);
}

void QtUndoRedo::undoReleased()
{
	if (m_pressed)
	{
		m_pressed = false;
		MessageHistoryUndo().dispatch();
	}
}

void QtUndoRedo::redoReleased()
{
	if (m_pressed)
	{
		m_pressed = false;
		MessageHistoryRedo().dispatch();
	}
}

void QtUndoRedo::showHistory()
{
	if (m_pressed)
	{
		m_pressed = false;

		if (m_historyList)
		{
			m_historyList->deleteLater();
			m_historyList = nullptr;
		}

		m_undoButton->setDown(false);
		m_redoButton->setDown(false);
		m_historyButton->setDown(false);

		m_undoButton->setAttribute(Qt::WA_UnderMouse, false);
		m_redoButton->setAttribute(Qt::WA_UnderMouse, false);
		m_historyButton->setAttribute(Qt::WA_UnderMouse, false);

		m_historyList = new QtHistoryList(m_history, m_currentIndex);

		QPoint pos = m_undoButton->mapToGlobal(m_undoButton->pos());

		m_historyList->showPopup(QPoint(pos.x(), pos.y() + m_undoButton->size().height() + 5));

		connect(m_historyList, &QtHistoryList::closed, this, &QtUndoRedo::hidHistory);
	}
}

void QtUndoRedo::hidHistory()
{
	if (m_historyList)
	{
		m_historyList->deleteLater();
		m_historyList = nullptr;

		m_historyHiddenAt = TimeStamp::now();
	}
}
