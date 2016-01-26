#include "qt/view/QtCodeView.h"

#include "utility/file/FileSystem.h"
#include "utility/ResourcePaths.h"
#include "qt/utility/utilityQt.h"

#include "qt/element/QtCodeArea.h"
#include "qt/element/QtCodeFileList.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ColorScheme.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtCodeView::doRefreshView, this))
	, m_showCodeSnippetsFunctor(std::bind(&QtCodeView::doShowCodeSnippets, this, std::placeholders::_1))
	, m_addCodeSnippetsFunctor(std::bind(&QtCodeView::doAddCodeSnippets, this, std::placeholders::_1, std::placeholders::_2))
	, m_showCodeFileFunctor(std::bind(&QtCodeView::doShowCodeFile, this, std::placeholders::_1))
	, m_doShowFirstActiveSnippetFunctor(std::bind(&QtCodeView::doShowFirstActiveSnippet, this))
	, m_doShowActiveTokenIdsFunctor(std::bind(&QtCodeView::doShowActiveTokenIds, this))
	, m_focusTokenIdsFunctor(std::bind(&QtCodeView::doFocusTokenIds, this, std::placeholders::_1))
	, m_defocusTokenIdsFunctor(std::bind(&QtCodeView::doDefocusTokenIds, this))
	, m_isExpanding(false)
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

void QtCodeView::addCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert)
{
	m_addCodeSnippetsFunctor(snippets, insert);
}

void QtCodeView::showCodeFile(const CodeSnippetParams& params)
{
	m_showCodeFileFunctor(params);
}

void QtCodeView::showFirstActiveSnippet()
{
	m_doShowFirstActiveSnippetFunctor();
}

void QtCodeView::showActiveTokenIds()
{
	m_doShowActiveTokenIdsFunctor();
}

void QtCodeView::focusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_focusTokenIdsFunctor(focusedTokenIds);
}

void QtCodeView::defocusTokenIds()
{
	m_defocusTokenIdsFunctor();
}

void QtCodeView::doRefreshView()
{
	setStyleSheet();
	m_widget->clearCodeSnippets();
	QtCodeArea::clearAnnotationColors();
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
			m_widget->addFile(params.locationFile, params.refCount, params.modificationTime);
		}
		else
		{
			m_widget->addCodeSnippet(
				params.startLineNumber,
				params.title,
				params.titleId,
				params.code,
				params.locationFile,
				params.refCount,
				params.modificationTime
			);
		}
	}

	setStyleSheet(); // so property "isLast" of QtCodeSnippet is computed correctly
}

void QtCodeView::doAddCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert)
{
	for (const CodeSnippetParams& params : snippets)
	{
		m_widget->addCodeSnippet(
			params.startLineNumber,
			params.title,
			params.titleId,
			params.code,
			params.locationFile,
			params.refCount,
			params.modificationTime,
			insert
		);
	}

	setStyleSheet(); // so property "isLast" of QtCodeSnippet is computed correctly

	if (m_isExpanding)
	{
		m_widget->scrollToFirstActiveSnippet();
		m_isExpanding = false;
	}
}

void QtCodeView::doShowCodeFile(const CodeSnippetParams& params)
{
	m_widget->addCodeSnippet(1, params.title, 0, params.code, params.locationFile, -1, params.modificationTime);
}

void QtCodeView::doShowFirstActiveSnippet()
{
	m_widget->setActiveTokenIds(m_activeTokenIds);

	if (!m_widget->scrollToFirstActiveSnippet())
	{
		m_widget->expandActiveSnippetFile();
		m_isExpanding = true;
	}
}

void QtCodeView::doShowActiveTokenIds()
{
	m_widget->setActiveTokenIds(m_activeTokenIds);
	m_widget->showActiveTokenIds();
}

void QtCodeView::doFocusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_widget->focusTokenIds(focusedTokenIds);
}

void QtCodeView::doDefocusTokenIds()
{
	m_widget->defocusTokenIds();
}

void QtCodeView::setStyleSheet() const
{
	utility::setWidgetBackgroundColor(m_widget, ColorScheme::getInstance()->getColor("code/background"));

	m_widget->setStyleSheet(utility::getStyleSheet(ResourcePaths::getGuiPath() + "code_view/code_view.css").c_str());
}
