#include "QtCodeView.h"

#include "CodeController.h"
#include "ResourcePaths.h"
#include "tracing.h"

#include "ColorScheme.h"
#include "QtCodeArea.h"
#include "QtCodeNavigator.h"
#include "QtHighlighter.h"
#include "QtViewWidgetWrapper.h"
#include "utilityQt.h"

QtCodeView::QtCodeView(ViewLayout* viewLayout): CodeView(viewLayout)
{
	m_widget = new QtCodeNavigator();
}

void QtCodeView::createWidgetWrapper()
{
	setWidgetWrapper(std::make_shared<QtViewWidgetWrapper>(m_widget));
}

void QtCodeView::refreshView()
{
	if (getController())
	{
		m_widget->setSchedulerId(getController()->getTabId());
	}

	m_onQtThread([=]() {
		TRACE("refresh");

		setStyleSheet();

		QtCodeArea::clearAnnotationColors();
		QtHighlighter::clearHighlightingRules();

		m_widget->clearCache();
	});
}

bool QtCodeView::isVisible() const
{
	return m_widget->isVisible();
}

void QtCodeView::findMatches(ScreenSearchSender* sender, const std::wstring& query)
{
	m_onQtThread([sender, query, this]() {
		size_t matchCount = m_widget->findScreenMatches(query);
		sender->foundMatches(this, matchCount);
	});
}

void QtCodeView::activateMatch(size_t matchIndex)
{
	m_onQtThread([matchIndex, this]() { m_widget->activateScreenMatch(matchIndex); });
}

void QtCodeView::deactivateMatch(size_t matchIndex)
{
	m_onQtThread([matchIndex, this]() { m_widget->deactivateScreenMatch(matchIndex); });
}

void QtCodeView::clearMatches()
{
	if (!m_widget->hasScreenMatches())
	{
		return;
	}

	m_onQtThread([this]() { m_widget->clearScreenMatches(); });
}

void QtCodeView::clear()
{
	m_onQtThread([=]() { m_widget->clear(); });
}

bool QtCodeView::showsErrors() const
{
	return m_widget->hasErrors();
}

void QtCodeView::showSnippets(
	const std::vector<CodeFileParams> files,
	const CodeParams params,
	const CodeScrollParams scrollParams)
{
	m_onQtThread([=]() {
		TRACE("show snippets");

		m_widget->setMode(QtCodeNavigator::MODE_LIST);

		if (params.clearSnippets)
		{
			m_widget->clearSnippets();
		}

		setNavigationState(params);

		for (const CodeFileParams& file: files)
		{
			m_widget->addSnippetFile(file);
		}

		m_widget->updateFiles();

		m_widget->scrollTo(scrollParams, !params.clearSnippets);
	});
}

void QtCodeView::showSingleFile(
	const CodeFileParams file, const CodeParams params, const CodeScrollParams scrollParams)
{
	m_onQtThread([=]() {
		TRACE("show single file");

		bool animatedScroll = !m_widget->isInListMode();

		m_widget->setMode(QtCodeNavigator::MODE_SINGLE);

		if (params.clearSnippets)
		{
			m_widget->clearSnippets();
		}

		setNavigationState(params);

		if (file.locationFile)
		{
			if (m_widget->addSingleFile(file, params.useSingleFileCache))
			{
				animatedScroll = false;
			}

			m_widget->updateFiles();
			m_widget->scrollTo(scrollParams, animatedScroll);
		}
		else
		{
			m_widget->clearFile();
		}
	});
}

void QtCodeView::updateSourceLocations(const std::vector<CodeFileParams> files)
{
	m_onQtThread([=]() {
		TRACE("update source locations");

		for (const CodeFileParams& file: files)
		{
			for (const CodeSnippetParams& snippet: file.snippetParams)
			{
				if (snippet.hasAllSourceLocations)
				{
					m_widget->updateSourceLocations(snippet);
				}
			}

			if (file.fileParams && file.fileParams->hasAllSourceLocations)
			{
				m_widget->updateSourceLocations(*file.fileParams.get());
			}
		}
	});
}

void QtCodeView::scrollTo(const CodeScrollParams params, bool animated)
{
	m_onQtThread([=]() { m_widget->scrollTo(params, animated); });
}

void QtCodeView::focusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	m_onQtThread([=]() { m_widget->focusTokenIds(focusedTokenIds); });
}

void QtCodeView::defocusTokenIds()
{
	m_onQtThread([=]() { m_widget->defocusTokenIds(); });
}

bool QtCodeView::isInListMode() const
{
	return m_widget->isInListMode();
}

void QtCodeView::setMode(bool listMode)
{
	m_widget->setMode(listMode ? QtCodeNavigator::MODE_LIST : QtCodeNavigator::MODE_SINGLE);
}

bool QtCodeView::hasSingleFileCached(const FilePath& filePath) const
{
	return m_widget->hasSingleFileCached(filePath);
}

void QtCodeView::setNavigationState(const CodeParams& params)
{
	m_widget->setActiveTokenIds(params.activeTokenIds);
	m_widget->setErrorInfos(params.errorInfos);

	if (params.activeLocationIds.size())
	{
		m_widget->setCurrentActiveLocationIds(params.activeLocationIds);
	}

	if (params.activeLocalSymbolIds.size())
	{
		if (params.activeLocalSymbolType == LOCATION_TOKEN)
		{
			m_widget->setCurrentActiveTokenIds(
				params.currentActiveLocalLocationIds.size() ? std::vector<Id>()
															: params.activeLocalSymbolIds);
		}

		m_widget->setActiveLocalTokenIds(params.activeLocalSymbolIds, params.activeLocalSymbolType);
	}

	if (params.currentActiveLocalLocationIds.size())
	{
		m_widget->setCurrentActiveLocalLocationIds(params.currentActiveLocalLocationIds);
	}

	m_widget->updateReferenceCount(
		params.referenceCount,
		params.referenceIndex,
		params.localReferenceCount,
		params.localReferenceIndex);
}

void QtCodeView::setStyleSheet() const
{
	utility::setWidgetBackgroundColor(
		m_widget, ColorScheme::getInstance()->getColor("code/background"));

	std::string styleSheet = utility::getStyleSheet(
		ResourcePaths::getGuiPath().concatenate(L"code_view/code_view.css"));

	m_widget->setStyleSheet(styleSheet.c_str());
}
