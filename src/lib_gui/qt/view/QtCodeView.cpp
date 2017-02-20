#include "qt/view/QtCodeView.h"

#include "utility/file/FileSystem.h"
#include "utility/ResourcePaths.h"
#include "qt/utility/utilityQt.h"

#include "qt/element/QtCodeArea.h"
#include "qt/element/QtCodeNavigator.h"
#include "qt/utility/QtHighlighter.h"
#include "qt/view/QtViewWidgetWrapper.h"
#include "settings/ColorScheme.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout)
	: CodeView(viewLayout)
	, m_showCodeSnippetsFunctor(
		std::bind(&QtCodeView::doShowCodeSnippets, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
	, m_addCodeSnippetsFunctor(std::bind(&QtCodeView::doAddCodeSnippets, this, std::placeholders::_1, std::placeholders::_2))
	, m_setFileStateFunctor(std::bind(&QtCodeView::doSetFileState, this, std::placeholders::_1, std::placeholders::_2))
	, m_doShowActiveSnippetFunctor(
		std::bind(&QtCodeView::doShowActiveSnippet, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3))
	, m_doShowActiveTokenIdsFunctor(std::bind(&QtCodeView::doShowActiveTokenIds, this, std::placeholders::_1))
	, m_doShowActiveLocalSymbolIdsFunctor(std::bind(&QtCodeView::doShowActiveLocalSymbolIds, this, std::placeholders::_1))
	, m_focusTokenIdsFunctor(std::bind(&QtCodeView::doFocusTokenIds, this, std::placeholders::_1))
{
	m_widget = new QtCodeNavigator();
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
	m_onQtThread(
		[=]()
		{
			setStyleSheet();

			m_widget->refreshStyle();

			QtCodeArea::clearAnnotationColors();
			QtHighlighter::clearHighlightingRules();
		}
	);
}

void QtCodeView::clear()
{
	m_onQtThread(
		[=]()
		{
			m_widget->clear();
		}
	);

	m_errorInfos.clear();
}

void QtCodeView::clearCodeSnippets()
{
	m_onQtThread(
		[=]()
		{
			m_widget->clearCodeSnippets();
		}
	);

	m_errorInfos.clear();
}

void QtCodeView::setErrorInfos(const std::vector<ErrorInfo>& errorInfos)
{
	m_errorInfos = errorInfos;
}

bool QtCodeView::showsErrors() const
{
	return m_errorInfos.size() > 0;
}

void QtCodeView::showCodeSnippets(
	const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds, bool setupFiles)
{
	m_showCodeSnippetsFunctor(snippets, activeTokenIds, setupFiles);
}

void QtCodeView::addCodeSnippets(const std::vector<CodeSnippetParams>& snippets, bool insert)
{
	m_addCodeSnippetsFunctor(snippets, insert);
}

void QtCodeView::setFileState(const FilePath filePath, FileState state)
{
	m_setFileStateFunctor(filePath, state);
}

void QtCodeView::showActiveSnippet(
	const std::vector<Id>& activeTokenIds, std::shared_ptr<TokenLocationCollection> collection, bool scrollTo)
{
	m_doShowActiveSnippetFunctor(activeTokenIds, collection, scrollTo);
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
	m_onQtThread(
		[=]()
		{
			m_widget->defocusTokenIds();
		}
	);
}

void QtCodeView::showContents()
{
	m_onQtThread(
		[=]()
		{
			m_widget->showContents();
		}
	);
}

void QtCodeView::scrollToValue(int value, bool inListMode)
{
	m_onQtThread(
		[=]()
		{
			m_widget->scrollToValue(value, inListMode);
		}
	);
}

void QtCodeView::scrollToLine(const FilePath filePath, unsigned int line)
{
	m_onQtThread(
		[=]()
		{
			m_widget->scrollToLine(filePath, line);
		}
	);
}

void QtCodeView::scrollToDefinition(bool ignoreActiveReference)
{
	m_onQtThread(
		[=]()
		{
			m_widget->scrollToDefinition(ignoreActiveReference);
		}
	);
}

void QtCodeView::doShowCodeSnippets(
	const std::vector<CodeSnippetParams>& snippets, const std::vector<Id>& activeTokenIds, bool setupFiles)
{
	m_widget->setActiveTokenIds(activeTokenIds);
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

	m_widget->addedFiles();

	if (setupFiles)
	{
		m_widget->setupFiles();
	}

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

	m_widget->scrollToSnippetIfRequested();
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

	m_widget->scrollToSnippetIfRequested();
}

void QtCodeView::doShowActiveSnippet(
	const std::vector<Id>& activeTokenIds, std::shared_ptr<TokenLocationCollection> collection, bool scrollTo)
{
	m_widget->showActiveSnippet(activeTokenIds, collection, scrollTo);
}

void QtCodeView::doShowActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	m_widget->setActiveTokenIds(activeTokenIds);
	m_widget->updateFiles();
}

void QtCodeView::doShowActiveLocalSymbolIds(const std::vector<Id>& localSymbolIds)
{
	m_widget->setActiveLocalSymbolIds(localSymbolIds);
	m_widget->updateFiles();
}

void QtCodeView::doFocusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_widget->focusTokenIds(focusedTokenIds);
}

void QtCodeView::setStyleSheet() const
{
	utility::setWidgetBackgroundColor(m_widget, ColorScheme::getInstance()->getColor("code/background"));

	std::string styleSheet = utility::getStyleSheet(ResourcePaths::getGuiPath() + "code_view/code_view.css");

	m_widget->setStyleSheet(styleSheet.c_str());
}
