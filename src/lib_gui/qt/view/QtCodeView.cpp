#include "qt/view/QtCodeView.h"

#include "utility/file/FileSystem.h"
#include "utility/ResourcePaths.h"
#include "qt/utility/utilityQt.h"

#include "qt/element/QtCodeArea.h"
#include "qt/element/QtCodeFileList.h"
#include "qt/utility/QtHighlighter.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ColorScheme.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
	, m_refreshViewFunctor(std::bind(&QtCodeView::doRefreshView, this))
	, m_clearFunctor(std::bind(&QtCodeView::doClear, this))
	, m_showCodeSnippetsFunctor(std::bind(&QtCodeView::doShowCodeSnippets, this, std::placeholders::_1, std::placeholders::_2))
	, m_addCodeSnippetsFunctor(std::bind(&QtCodeView::doAddCodeSnippets, this, std::placeholders::_1, std::placeholders::_2))
	, m_showCodeFileFunctor(std::bind(&QtCodeView::doShowCodeFile, this, std::placeholders::_1))
	, m_setFileStateFunctor(std::bind(&QtCodeView::doSetFileState, this, std::placeholders::_1, std::placeholders::_2))
	, m_doShowFirstActiveSnippetFunctor(std::bind(&QtCodeView::doShowFirstActiveSnippet, this, std::placeholders::_1, std::placeholders::_2))
	, m_doShowActiveTokenIdsFunctor(std::bind(&QtCodeView::doShowActiveTokenIds, this, std::placeholders::_1))
	, m_doShowActiveLocalSymbolIdsFunctor(std::bind(&QtCodeView::doShowActiveLocalSymbolIds, this, std::placeholders::_1))
	, m_focusTokenIdsFunctor(std::bind(&QtCodeView::doFocusTokenIds, this, std::placeholders::_1))
	, m_defocusTokenIdsFunctor(std::bind(&QtCodeView::doDefocusTokenIds, this))
	, m_showContentsFunctor(std::bind(&QtCodeView::doShowContents, this))
	, m_scrollToValueFunctor(std::bind(&QtCodeView::doScrollToValue, this, std::placeholders::_1))
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

void QtCodeView::clear()
{
	m_clearFunctor();

	m_errorInfos.clear();
	m_activeTokenIds.clear();
}

void QtCodeView::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	m_activeTokenIds = activeTokenIds;
}

void QtCodeView::setErrorInfos(const std::vector<ErrorInfo>& errorInfos)
{
	m_errorInfos = errorInfos;
}

void QtCodeView::showCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds)
{
	m_showCodeSnippetsFunctor(snippets, activeTokenIds);
}

void QtCodeView::addCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert)
{
	m_addCodeSnippetsFunctor(snippets, insert);
}

void QtCodeView::showCodeFile(const CodeSnippetParams& params)
{
	m_showCodeFileFunctor(params);
}

void QtCodeView::setFileState(const FilePath filePath, FileState state)
{
	m_setFileStateFunctor(filePath, state);
}

void QtCodeView::showFirstActiveSnippet(const std::vector<Id>& activeTokenIds, bool scrollTo)
{
	m_doShowFirstActiveSnippetFunctor(activeTokenIds, scrollTo);
}

void QtCodeView::showActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	m_doShowActiveTokenIdsFunctor(activeTokenIds);
}

void QtCodeView::showActiveLocalSymbolIds(const std::vector<Id>& activeLocalSymbolIds)
{
	m_doShowActiveLocalSymbolIdsFunctor(activeLocalSymbolIds);
}

void QtCodeView::focusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_focusTokenIdsFunctor(focusedTokenIds);
}

void QtCodeView::defocusTokenIds()
{
	m_defocusTokenIdsFunctor();
}

void QtCodeView::showContents()
{
	m_showContentsFunctor();
}

void QtCodeView::scrollToValue(int value)
{
	m_scrollToValueFunctor(value);
}

void QtCodeView::doRefreshView()
{
	setStyleSheet();
	m_widget->clearCodeSnippets();

	QtCodeArea::clearAnnotationColors();
	QtHighlighter::clearHighlightingRules();
}

void QtCodeView::doClear()
{
	m_widget->clearCodeSnippets();
}

void QtCodeView::doShowCodeSnippets(const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds)
{
	setActiveTokenIds(activeTokenIds);

	m_widget->setActiveTokenIds(m_activeTokenIds);
	m_widget->setErrorInfos(m_errorInfos);

	for (const CodeSnippetParams& params : snippets)
	{
		if (params.isCollapsed)
		{
			m_widget->addFile(params.locationFile, params.refCount, params.modificationTime);
		}
		else
		{
			m_widget->addCodeSnippet(params);
		}
	}

	m_widget->updateFiles();

	setStyleSheet(); // so property "isLast" of QtCodeSnippet is computed correctly
}

void QtCodeView::doAddCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert)
{
	for (const CodeSnippetParams& params : snippets)
	{
		m_widget->addCodeSnippet(params, insert);
	}

	m_widget->updateFiles();

	setStyleSheet(); // so property "isLast" of QtCodeSnippet is computed correctly

	m_widget->scrollToActiveFileIfRequested();
}

void QtCodeView::doShowCodeFile(const CodeSnippetParams& params)
{
	m_widget->addCodeSnippet(params);
}

void QtCodeView::doSetFileState(const FilePath filePath, FileState state)
{
	switch (state)
	{
	case FILE_MINIMIZED:
		m_widget->setFileMinimized(filePath);
		break;
	case FILE_SNIPPETS:
		m_widget->setFileSnippets(filePath);
		break;
	case FILE_MAXIMIZED:
		m_widget->setFileMaximized(filePath);
		break;
	}
}

void QtCodeView::doShowFirstActiveSnippet(const std::vector<Id>& activeTokenIds, bool scrollTo)
{
	m_widget->setActiveTokenIds(activeTokenIds);
	m_widget->showFirstActiveSnippet(scrollTo);
}

void QtCodeView::doShowActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	m_widget->setActiveTokenIds(activeTokenIds);
	m_widget->showActiveTokenIds();
}

void QtCodeView::doShowActiveLocalSymbolIds(const std::vector<Id>& localSymbolIds)
{
	m_widget->setActiveLocalSymbolIds(localSymbolIds);
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

void QtCodeView::doShowContents()
{
	m_widget->showContents();
}

void QtCodeView::doScrollToValue(int value)
{
	m_widget->scrollToValue(value);
}

void QtCodeView::setStyleSheet() const
{
	utility::setWidgetBackgroundColor(m_widget, ColorScheme::getInstance()->getColor("code/background"));

	std::string styleSheet = utility::getStyleSheet(ResourcePaths::getGuiPath() + "code_view/code_view.css");

	m_widget->setStyleSheet(styleSheet.c_str());
}
