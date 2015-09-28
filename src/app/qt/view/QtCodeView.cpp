#include "qt/view/QtCodeView.h"

#include "utility/file/FileSystem.h"
#include "qt/utility/utilityQt.h"

#include "qt/element/QtCodeFileList.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ColorScheme.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtCodeView::doRefreshView, this))
	, m_showCodeSnippetsFunctor(std::bind(&QtCodeView::doShowCodeSnippets, this, std::placeholders::_1))
	, m_addCodeSnippetsFunctor(std::bind(&QtCodeView::doAddCodeSnippets, this, std::placeholders::_1))
	, m_showCodeFileFunctor(std::bind(&QtCodeView::doShowCodeFile, this, std::placeholders::_1))
	, m_doShowFirstActiveSnippetFunctor(std::bind(&QtCodeView::doShowFirstActiveSnippet, this))
	, m_focusTokenFunctor(std::bind(&QtCodeView::doFocusToken, this, std::placeholders::_1))
	, m_defocusTokenFunctor(std::bind(&QtCodeView::doDefocusToken, this))
{
	m_widget = new QtCodeFileList();
	setStyleSheet();
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

void QtCodeView::addCodeSnippets(const std::vector<CodeSnippetParams>& snippets)
{
	m_addCodeSnippetsFunctor(snippets);
}

void QtCodeView::showCodeFile(const CodeSnippetParams& params)
{
	m_showCodeFileFunctor(params);
}

void QtCodeView::showFirstActiveSnippet()
{
	m_doShowFirstActiveSnippetFunctor();
}

void QtCodeView::focusToken(const Id tokenId)
{
	m_focusTokenFunctor(tokenId);
}

void QtCodeView::defocusToken()
{
	m_defocusTokenFunctor();
}

void QtCodeView::doRefreshView()
{
	setStyleSheet();
	m_widget->clearCodeSnippets();
}

void QtCodeView::doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets)
{
	m_widget->clearCodeSnippets();

	m_widget->setActiveTokenIds(m_activeTokenIds);
	m_widget->setErrorMessages(m_errorMessages);

	for (const CodeSnippetParams& params : snippets)
	{
		if (params.isCollapsed)
		{
			m_widget->addFile(params.locationFile, params.refCount);
		}
		else
		{
			m_widget->addCodeSnippet(
				params.startLineNumber,
				params.title,
				params.titleId,
				params.code,
				params.locationFile,
				params.refCount
			);
		}
	}

	setStyleSheet(); // so property "isLast" of QtCodeSnippet is computed correctly
}

void QtCodeView::doAddCodeSnippets(const std::vector<CodeSnippetParams>& snippets)
{
	for (const CodeSnippetParams& snippet : snippets)
	{
		m_widget->addCodeSnippet(
			snippet.startLineNumber,
			snippet.title,
			snippet.titleId,
			snippet.code,
			snippet.locationFile,
			snippet.refCount,
			true
		);
	}

	setStyleSheet(); // so property "isLast" of QtCodeSnippet is computed correctly
}

void QtCodeView::doShowCodeFile(const CodeSnippetParams& params)
{
	m_widget->addCodeSnippet(1, params.title, 0, params.code, params.locationFile, params.refCount);
}

void QtCodeView::doShowFirstActiveSnippet()
{
	m_widget->setActiveTokenIds(m_activeTokenIds);

	if (!m_widget->scrollToFirstActiveSnippet())
	{
		m_widget->expandActiveSnippetFile();
	}
}

void QtCodeView::doFocusToken(const Id tokenId)
{
	m_widget->focusToken(tokenId);
}

void QtCodeView::doDefocusToken()
{
	m_widget->defocusToken();
}

void QtCodeView::setStyleSheet() const
{
	utility::setWidgetBackgroundColor(m_widget, ColorScheme::getInstance()->getColor("code/background"));

	m_widget->setStyleSheet(utility::getStyleSheet("data/gui/code_view/code_view.css").c_str());
}
