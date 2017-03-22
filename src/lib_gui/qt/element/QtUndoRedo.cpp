#include "qt/element/QtUndoRedo.h"

#include <QPushButton>
#include <QHBoxLayout>

#include "utility/messaging/type/MessageUndo.h"
#include "utility/messaging/type/MessageRedo.h"
#include "utility/ResourcePaths.h"

#include "qt/utility/utilityQt.h"
#include "qt/utility/QtContextMenu.h"
#include "settings/ApplicationSettings.h"

QtUndoRedo::QtUndoRedo()
{
	setObjectName("undo_redo_bar");

	m_undoButton = new QPushButton(this);
	m_undoButton->setObjectName("undo_button");
	m_undoButton->setToolTip("back");
	m_undoButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_undoButton->setEnabled(false);

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
	layout->addWidget(m_redoButton);

    connect(m_undoButton, SIGNAL(clicked()), this, SLOT(undo()));
    connect(m_redoButton, SIGNAL(clicked()), this, SLOT(redo()));

    refreshStyle();
}

QtUndoRedo::~QtUndoRedo()
{
}

void QtUndoRedo::undo()
{
    MessageUndo().dispatch();
}

void QtUndoRedo::redo()
{
    MessageRedo().dispatch();
}

void QtUndoRedo::setUndoButtonEnabled(bool enabled)
{
    m_undoButton->setEnabled(enabled);
	QtContextMenu::enableUndo(enabled);

}

void QtUndoRedo::setRedoButtonEnabled(bool enabled)
{
    m_redoButton->setEnabled(enabled);
	QtContextMenu::enableRedo(enabled);
}

void QtUndoRedo::refreshStyle()
{
	float height = std::max(ApplicationSettings::getInstance()->getFontSize() + 16, 30);

	m_undoButton->setFixedHeight(height);
	m_redoButton->setFixedHeight(height);

	m_undoButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath() + "undoredo_view/images/arrow_left.png",
		"search/button"
	));

	m_redoButton->setIcon(utility::createButtonIcon(
		ResourcePaths::getGuiPath() + "undoredo_view/images/arrow_right.png",
		"search/button"
	));
}
