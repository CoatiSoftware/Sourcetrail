#include "qt/view/QtCodeView.h"

#include <QScrollBar>

#include "qt/element/QtCodeFileList.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "utility/file/FileSystem.h"
#include "utility/text/TextAccess.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtCodeView::doRefreshView, this))
	, m_showCodeSnippetsFunctor(std::bind(&QtCodeView::doShowCodeSnippets, this, std::placeholders::_1))
	, m_showCodeFileFunctor(std::bind(&QtCodeView::doShowCodeFile, this, std::placeholders::_1))
{
	m_widget = new QtCodeFileList();
	setStyleSheet(m_widget);
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

void QtCodeView::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	m_activeTokenIds = activeTokenIds;
}

void QtCodeView::setErrorMessages(const std::vector<std::string>& errorMessages)
{
	m_errorMessages = errorMessages;
}

void QtCodeView::showCodeSnippets(const std::vector<CodeSnippetParams>& snippets)
{
	m_showCodeSnippetsFunctor(snippets);
}

void QtCodeView::showCodeFile(const CodeSnippetParams& params)
{
	m_showCodeFileFunctor(params);
}

void QtCodeView::doRefreshView()
{
	setStyleSheet(m_widget);

	clearClosedWindows();
	for (std::shared_ptr<QtCodeFileList> window: m_windows)
	{
		setStyleSheet(window.get());
	}
}

void QtCodeView::doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets)
{
	m_widget->clearCodeSnippets();

	clearClosedWindows();
	for (std::shared_ptr<QtCodeFileList> window: m_windows)
	{
		if (m_errorMessages.size())
		{
			window->close();
		}
		else
		{
			window->setActiveTokenIds(m_activeTokenIds);
			window->setErrorMessages(m_errorMessages);
		}
	}

	m_widget->setActiveTokenIds(m_activeTokenIds);
	m_widget->setErrorMessages(m_errorMessages);
	m_widget->setShowMaximizeButton(m_errorMessages.size() == 0);

	for (const CodeSnippetParams& params : snippets)
	{
		m_widget->addCodeSnippet(params.startLineNumber, params.code, params.locationFile);
	}
}

void QtCodeView::doShowCodeFile(const CodeSnippetParams& params)
{
	std::shared_ptr<QtCodeFileList> ptr = createQtCodeFileList();
	m_windows.push_back(ptr);

	ptr->setShowMaximizeButton(false);
	ptr->setActiveTokenIds(m_activeTokenIds);
	ptr->setErrorMessages(m_errorMessages);
	ptr->addCodeSnippet(1, params.code, params.locationFile);

	ptr->setWindowTitle(params.locationFile.getFilePath().fileName().c_str());
	ptr->show();

	float percent = float(params.startLineNumber + params.endLineNumber) / float(params.lineCount) / 2;
	float min = ptr->verticalScrollBar()->minimum();
	float max = ptr->verticalScrollBar()->maximum();

	ptr->verticalScrollBar()->setValue(min + (max - min) * percent);
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
