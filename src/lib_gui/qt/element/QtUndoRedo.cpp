#include "qt/element/QtUndoRedo.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QTimer>

#include "utility/messaging/type/MessageUndo.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/ResourcePaths.h"

#include "qt/element/QtHistoryList.h"
#include "qt/utility/utilityQt.h"
#include "qt/utility/QtContextMenu.h"
#include "settings/ApplicationSettings.h"

QtUndoRedo::QtUndoRedo()
	: m_pressed(false)
	, m_historyList(nullptr)
	, m_historyHiddenAt(TimeStamp::now())
{
	setObjectName("undo_redo_bar");

	m_undoButton = new QPushButton(this);
	m_undoButton->setObjectName("undo_button");
	m_undoButton->setToolTip("back");
	m_undoButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_undoButton->setEnabled(false);

	m_historyButton = new QPushButton(this);
	m_historyButton->setObjectName("history_button");
	m_historyButton->setToolTip("history");
	m_historyButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_historyButton->setEnabled(false);

	m_redoButton = new QPushButton(this);
	m_redoButton->setObjectName("redo_button");
	m_redoButton->setToolTip("forward");
	m_redoButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
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

	refreshStyle();
}

QtUndoRedo::~QtUndoRedo()
{
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
		MessageUndo().dispatch();
	}
}

void QtUndoRedo::redoReleased()
{
	if (m_pressed)
	{
		m_pressed = false;
		MessageRedo().dispatch();
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

void QtUndoRedo::refreshStyle()
{
	float height = std::max(ApplicationSettings::getInstance()->getFontSize() + 16, 30);

	m_undoButton->setFixedHeight(height);
	m_redoButton->setFixedHeight(height);
	m_historyButton->setFixedHeight(height);

	m_undoButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "undoredo_view/images/arrow_left.png",
		"search/button"
	));

	m_redoButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "undoredo_view/images/arrow_right.png",
		"search/button"
	));

	m_historyButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath().str() + "undoredo_view/images/history.png",
		"search/button"
	));

	int iconSize = int(height / 4) * 2 + 2;

	m_undoButton->setIconSize(QSize(iconSize, iconSize));
	m_redoButton->setIconSize(QSize(iconSize, iconSize));
	m_historyButton->setIconSize(QSize(iconSize, iconSize));
}
