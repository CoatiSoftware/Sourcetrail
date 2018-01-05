#include "qt/view/QtUndoRedoView.h"

#include "utility/ResourcePaths.h"
#include "qt/utility/utilityQt.h"

#include "qt/view/QtViewWidgetWrapper.h"

QtUndoRedoView::QtUndoRedoView(ViewLayout* viewLayout)
	: UndoRedoView(viewLayout)
{
	m_widget = new QtUndoRedo();
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
	m_onQtThread(
		[=]()
		{
			setStyleSheet();
			m_widget->refreshStyle();
		}
	);
}

void QtUndoRedoView::setRedoButtonEnabled(bool enabled)
{
	m_onQtThread(
		[=]()
		{
			m_widget->setRedoButtonEnabled(enabled);
		}
	);
}

void QtUndoRedoView::setUndoButtonEnabled(bool enabled)
{
	m_onQtThread(
		[=]()
		{
			m_widget->setUndoButtonEnabled(enabled);
		}
	);
}

void QtUndoRedoView::updateHistory(const std::vector<SearchMatch>& searchMatches, size_t currentIndex)
{
	m_onQtThread(
		[=]()
		{
			m_widget->updateHistory(searchMatches, currentIndex);
		}
	);
}

void QtUndoRedoView::setStyleSheet()
{
	m_widget->setStyleSheet(utility::getStyleSheet(
		ResourcePaths::getGuiPath().concatenate(FilePath("undoredo_view/undoredo_view.css"))).c_str());
}
