#include "qt/view/QtUndoRedoView.h"

#include "qt/view/QtMainView.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/text/TextAccess.h"

QtUndoRedoView::QtUndoRedoView(ViewLayout* viewLayout)
	: UndoRedoView(viewLayout)
	, m_setRedoButtonEnabledFunctor(std::bind(&QtUndoRedoView::doSetRedoButtonEnabled, this, std::placeholders::_1))
	, m_setUndoButtonEnabledFunctor(std::bind(&QtUndoRedoView::doSetUndoButtonEnabled, this, std::placeholders::_1))
{
	m_widget = std::make_shared<QtUndoRedo>();
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
}


void QtUndoRedoView::setStyleSheet()
{
	std::string css = TextAccess::createFromFile("data/gui/undoredo_view/undoredo_view.css")->getText();

	m_widget->setStyleSheet(css.c_str());

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
