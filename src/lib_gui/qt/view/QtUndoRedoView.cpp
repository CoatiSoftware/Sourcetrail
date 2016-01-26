#include "qt/view/QtUndoRedoView.h"

#include "qt/utility/utilityQt.h"

#include "qt/view/QtMainView.h"
#include "qt/view/QtViewWidgetWrapper.h"

#include "utility/ResourcePaths.h"

QtUndoRedoView::QtUndoRedoView(ViewLayout* viewLayout)
	: UndoRedoView(viewLayout)
	, m_refreshFunctor(std::bind(&QtUndoRedoView::doRefreshView, this))
	, m_setRedoButtonEnabledFunctor(std::bind(&QtUndoRedoView::doSetRedoButtonEnabled, this, std::placeholders::_1))
	, m_setUndoButtonEnabledFunctor(std::bind(&QtUndoRedoView::doSetUndoButtonEnabled, this, std::placeholders::_1))
{
	m_widget = new QtUndoRedo();
	setStyleSheet();
}

QtUndoRedoView::~QtUndoRedoView()
{
}

void QtUndoRedoView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtUndoRedoView::initView()
{
}

void QtUndoRedoView::refreshView()
{
	m_refreshFunctor();
}

void QtUndoRedoView::setStyleSheet()
{
	m_widget->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "undoredo_view/undoredo_view.css").c_str());
}

void QtUndoRedoView::doRefreshView()
{
	setStyleSheet();
	m_widget->refreshStyle();
}

void QtUndoRedoView::doSetRedoButtonEnabled(bool enabled)
{
    m_widget->setRedoButtonEnabled(enabled);
}

void QtUndoRedoView::doSetUndoButtonEnabled(bool enabled)
{
    m_widget->setUndoButtonEnabled(enabled);
}

void QtUndoRedoView::setRedoButtonEnabled(bool enabled)
{
    m_setRedoButtonEnabledFunctor(enabled);
}

void QtUndoRedoView::setUndoButtonEnabled(bool enabled)
{
    m_setUndoButtonEnabledFunctor(enabled);
}
