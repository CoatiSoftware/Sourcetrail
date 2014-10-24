#include "qt/view/QtCodeView.h"

#include <QScrollBar>

#include "qt/element/QtCodeFileList.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/FileSystem.h"
#include "utility/text/TextAccess.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtCodeView::doRefreshView, this))
	, m_clearCodeSnippetsFunctor(std::bind(&QtCodeView::doClearCodeSnippets, this))
	, m_showCodeFileFunctor(std::bind(&QtCodeView::doShowCodeFile, this, std::placeholders::_1))
	, m_addCodeSnippetFunctor(std::bind(&QtCodeView::doAddCodeSnippet, this, std::placeholders::_1))
{
	m_widget = createQtCodeFileList();
}

QtCodeView::~QtCodeView()
{
}

void QtCodeView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtCodeView::initView()
{
}

void QtCodeView::refreshView()
{
	m_refreshViewFunctor();
}

void QtCodeView::showCodeFile(const CodeSnippetParams& params)
{
	m_showCodeFileFunctor(params);
}

void QtCodeView::addCodeSnippet(const CodeSnippetParams& params)
{
	m_addCodeSnippetFunctor(params);
}

void QtCodeView::clearCodeSnippets()
{
	m_clearCodeSnippetsFunctor();
}

void QtCodeView::doRefreshView()
{
	setStyleSheet(m_widget.get());

	clearClosedWindows();
	for (std::shared_ptr<QtCodeFileList> window: m_windows)
	{
		setStyleSheet(window.get());
	}
}

void QtCodeView::doShowCodeFile(const CodeSnippetParams& params)
{
	std::shared_ptr<QtCodeFileList> ptr = createQtCodeFileList();
	m_windows.push_back(ptr);

	ptr->setShowMaximizeButton(false);
	ptr->addCodeSnippet(1, params.code, params.locationFile, m_activeTokenIds);

	ptr->setWindowTitle(FileSystem::fileName(params.locationFile.getFilePath()).c_str());
	ptr->show();

	float percent = float(params.startLineNumber + params.endLineNumber) / float(params.lineCount) / 2;
	float min = ptr->verticalScrollBar()->minimum();
	float max = ptr->verticalScrollBar()->maximum();

	ptr->verticalScrollBar()->setValue(min + (max - min) * percent);
}

void QtCodeView::doAddCodeSnippet(const CodeSnippetParams& params)
{
	m_widget->addCodeSnippet(params.startLineNumber, params.code, params.locationFile, m_activeTokenIds);

	clearClosedWindows();
	for (std::shared_ptr<QtCodeFileList> window: m_windows)
	{
		window->setActiveTokenIds(m_activeTokenIds);
	}
}

void QtCodeView::doClearCodeSnippets()
{
	m_widget->clearCodeSnippets();
}

std::shared_ptr<QtCodeFileList> QtCodeView::createQtCodeFileList() const
{
	std::shared_ptr<QtCodeFileList> ptr = std::make_shared<QtCodeFileList>();
	setStyleSheet(ptr.get());
	return ptr;
}

void QtCodeView::setStyleSheet(QWidget* widget) const
{
	widget->setStyleSheet(TextAccess::createFromFile("data/gui/code_view/code_view.css")->getText().c_str());
}

void QtCodeView::setActiveTokenIds(std::vector<Id> ids)
{
	m_activeTokenIds = ids;		
}

void QtCodeView::clearClosedWindows()
{
	for (size_t i = 0; i < m_windows.size(); i++)
	{
		if (!m_windows[i]->isVisible())
		{
			m_windows.erase(m_windows.begin() + i);
			i--;
		}
	}
}
