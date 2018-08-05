#include "qt/element/QtCodeFile.h"

#include <QVBoxLayout>

#include "utility/messaging/type/MessageChangeFileView.h"

#include "data/location/SourceLocationFile.h"
#include "qt/element/QtCodeFileTitleBar.h"
#include "qt/element/QtCodeNavigator.h"
#include "qt/element/QtCodeSnippet.h"

QtCodeFile::QtCodeFile(const FilePath& filePath, QtCodeNavigator* navigator)
	: QFrame()
	, m_navigator(navigator)
	, m_filePath(filePath)
	, m_isWholeFile(false)
	, m_contentRequested(false)
{
	setObjectName("code_file");
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_titleBar = new QtCodeFileTitleBar();
	layout->addWidget(m_titleBar);

	if (!m_filePath.empty())
	{
		m_titleBar->getTitleButton()->setFilePath(filePath);
	}

	connect(m_titleBar, &QtCodeFileTitleBar::minimize, this, &QtCodeFile::clickedMinimizeButton);
	connect(m_titleBar, &QtCodeFileTitleBar::snippet, this, &QtCodeFile::clickedSnippetButton);
	connect(m_titleBar, &QtCodeFileTitleBar::maximize, this, &QtCodeFile::clickedMaximizeButton);

	m_snippetLayout = new QVBoxLayout();
	m_snippetLayout->setContentsMargins(0, 0, 0, 0);
	m_snippetLayout->setSpacing(0);
	layout->addLayout(m_snippetLayout);

	update();
}

QtCodeFile::~QtCodeFile()
{
}

void QtCodeFile::setModificationTime(const TimeStamp modificationTime)
{
	m_titleBar->getTitleButton()->setModificationTime(modificationTime);
}

const FilePath& QtCodeFile::getFilePath() const
{
	return m_filePath;
}

const QtCodeFileTitleBar* QtCodeFile::getTitleBar() const
{
	return m_titleBar;
}

QtCodeSnippet* QtCodeFile::addCodeSnippet(const CodeSnippetParams& params)
{
	if (m_isWholeFile && m_snippets.size() == 1)
	{
		return m_snippets[0];
	}

	for (QtCodeSnippet* snippet : m_snippets)
	{
		if (snippet->getStartLineNumber() == params.startLineNumber &&
			snippet->getEndLineNumber() == params.endLineNumber)
		{
			return snippet;
		}
	}

	QtCodeSnippet* snippet = new QtCodeSnippet(params, m_navigator, this);

	if (params.reduced)
	{
		m_titleBar->getTitleButton()->setProject(params.title);
		m_isWholeFile = true;
	}

	if (params.locationFile->isWhole() || m_isWholeFile)
	{
		m_isWholeFile = true;

		snippet->setIsActiveFile(true);

		if (params.refCount != -1)
		{
			updateRefCount(0);
		}

		for (QtCodeSnippet* oldSnippet : m_snippets)
		{
			oldSnippet->hide();
		}
		m_snippets.clear();
	}
	else
	{
		updateRefCount(params.refCount);
	}

	m_snippetLayout->addWidget(snippet);
	m_snippets.push_back(snippet);

	setSnippets();

	return snippet;
}

QtCodeSnippet* QtCodeFile::insertCodeSnippet(const CodeSnippetParams& params)
{
	QtCodeSnippet* newSnippet = new QtCodeSnippet(params, m_navigator, this);

	size_t i = 0;
	while (i < m_snippets.size())
	{
		uint start = newSnippet->getStartLineNumber();
		uint end = newSnippet->getEndLineNumber();

		QtCodeSnippet* oldSnippet = m_snippets[i];

		if (oldSnippet->getEndLineNumber() + 1 < start) // before
		{
			i++;
			continue;
		}
		else if (oldSnippet->getStartLineNumber() > end + 1) // after
		{
			break;
		}
		else if (oldSnippet->getStartLineNumber() <= start && oldSnippet->getEndLineNumber() >= end) // containing
		{
			newSnippet->deleteLater();
			return oldSnippet;
		}
		else if (oldSnippet->getStartLineNumber() < start || oldSnippet->getEndLineNumber() > end) // overlaping
		{
			m_navigator->clearSnippetReferences();
			newSnippet = QtCodeSnippet::merged(newSnippet, oldSnippet, m_navigator, this);
		}
		else if (oldSnippet->getStartLineNumber() >= start && oldSnippet->getEndLineNumber() <= end) // enclosing
		{
			// copy all source locations from old to new snippet: fulltext locations got lost
			oldSnippet->getArea()->getSourceLocationFile()->copySourceLocations(
				newSnippet->getArea()->getSourceLocationFile());
			newSnippet->getArea()->updateSourceLocations(oldSnippet->getArea()->getSourceLocationFile());
		}

		oldSnippet->hide();
		m_snippetLayout->removeWidget(oldSnippet);
		oldSnippet->deleteLater();

		m_snippets.erase(m_snippets.begin() + i);
	}

	m_snippetLayout->insertWidget(i, newSnippet);
	m_snippets.insert(m_snippets.begin() + i, newSnippet);

	setSnippets();

	return newSnippet;
}

void QtCodeFile::updateCodeSnippet(const CodeSnippetParams& params)
{
	if (m_isWholeFile && m_snippets.size() == 1)
	{
		m_snippets[0]->updateCodeSnippet(params);
		return;
	}

	for (QtCodeSnippet* snippet : m_snippets)
	{
		if (snippet->getStartLineNumber() == params.startLineNumber &&
			snippet->getEndLineNumber() == params.endLineNumber)
		{
			snippet->updateCodeSnippet(params);
		}
	}
}

std::vector<QtCodeSnippet*> QtCodeFile::getVisibleSnippets() const
{
	std::vector<QtCodeSnippet*> snippets;

	for (QtCodeSnippet* snippet : m_snippets)
	{
		if (snippet->isVisible())
		{
			snippets.push_back(snippet);
		}
	}

	return snippets;
}

QtCodeSnippet* QtCodeFile::getSnippetForLocationId(Id locationId) const
{
	for (QtCodeSnippet* snippet : m_snippets)
	{
		if (snippet->getLineNumberForLocationId(locationId))
		{
			return snippet;
		}
	}

	return nullptr;
}

QtCodeSnippet* QtCodeFile::getSnippetForLine(unsigned int line) const
{
	for (QtCodeSnippet* snippet : m_snippets)
	{
		if (snippet->getStartLineNumber() <= line && line <= snippet->getEndLineNumber())
		{
			return snippet;
		}
	}

	return nullptr;
}

std::pair<QtCodeSnippet*, Id> QtCodeFile::getFirstSnippetWithActiveLocationId(Id tokenId) const
{
	std::pair<QtCodeSnippet*, Id> result(nullptr, 0);

	for (QtCodeSnippet* snippet : m_snippets)
	{
		Id locationId = snippet->getFirstActiveLocationId(tokenId);
		if (locationId != 0)
		{
			result.first = snippet;
			result.second = locationId;
			break;
		}
	}

	return result;
}

bool QtCodeFile::isCollapsed() const
{
	return !m_snippets.size();
}

void QtCodeFile::requestContent()
{
	if (!isCollapsed() || m_contentRequested)
	{
		updateContent();
		return;
	}

	m_contentRequested = true;

	MessageChangeFileView::FileState state =
		m_isWholeFile ? MessageChangeFileView::FILE_MAXIMIZED : MessageChangeFileView::FILE_SNIPPETS;

	bool needsData = (m_snippets.size() == 0);

	MessageChangeFileView(m_filePath, state, MessageChangeFileView::VIEW_LIST, needsData, m_navigator->hasErrors()).dispatch();
}

void QtCodeFile::requestWholeFileContent()
{
	if (!m_isWholeFile)
	{
		MessageChangeFileView(
			m_filePath,
			MessageChangeFileView::FILE_MAXIMIZED,
			MessageChangeFileView::VIEW_LIST,
			true,
			m_navigator->hasErrors()
		).dispatch();
	}
	else
	{
		setSnippets();
	}
}

void QtCodeFile::updateContent()
{
	updateSnippets();

	for (QtCodeSnippet* snippet : m_snippets)
	{
		snippet->updateContent();
	}
}

void QtCodeFile::setWholeFile(bool isWholeFile, int refCount)
{
	m_isWholeFile = isWholeFile;
	setMinimized();

	updateRefCount(isWholeFile ? 0 : refCount);
}

void QtCodeFile::setIsComplete(bool isComplete)
{
	m_titleBar->setIsComplete(isComplete);
}

void QtCodeFile::setIsIndexed(bool isIndexed)
{
	m_titleBar->setIsIndexed(isIndexed);
}

void QtCodeFile::setMinimized()
{
	for (QtCodeSnippet* snippet : m_snippets)
	{
		snippet->hide();
	}

	m_titleBar->setMinimized();

	setStyleSheet("#code_file { padding-bottom: 0; } #code_file #title_bar { border-radius: 7px; }");
}

void QtCodeFile::setSnippets()
{
	for (QtCodeSnippet* snippet : m_snippets)
	{
		snippet->show();
	}

	m_titleBar->setSnippets();

	setStyleSheet("");
}

void QtCodeFile::setMaximized()
{
	setSnippets();
}

bool QtCodeFile::hasSnippets() const
{
	return m_snippets.size() > 0;
}

void QtCodeFile::updateSnippets()
{
	if (m_snippets.size() == 0)
	{
		return;
	}

	int maxDigits = 1;
	for (QtCodeSnippet* snippet : m_snippets)
	{
		if (snippet != m_snippets.front() && snippet->styleSheet().size())
		{
			snippet->setStyleSheet("");
		}

		maxDigits = qMax(maxDigits, snippet->lineNumberDigits());
	}

	for (QtCodeSnippet* snippet : m_snippets)
	{
		snippet->updateLineNumberAreaWidthForDigits(maxDigits);
	}

	if (!m_snippets.front()->styleSheet().size())
	{
		m_snippets.front()->setStyleSheet("#code_snippet { border: none; }");
	}
}

void QtCodeFile::updateTitleBar()
{
	m_titleBar->getTitleButton()->updateTexts();
}

void QtCodeFile::findScreenMatches(const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches)
{
	for (QtCodeSnippet* snippet : m_snippets)
	{
		if (snippet->isVisible())
		{
			snippet->findScreenMatches(query, screenMatches);
		}
	}
}

std::vector<std::pair<FilePath, Id>> QtCodeFile::getLocationIdsForTokenIds(const std::set<Id>& tokenIds) const
{
	std::vector<std::pair<FilePath, Id>> locationIds;
	for (QtCodeSnippet* snippet : m_snippets)
	{
		for (Id locationId : snippet->getLocationIdsForTokenIds(tokenIds))
		{
			locationIds.push_back(std::make_pair(m_filePath, locationId));
		}
	}
	return locationIds;
}

void QtCodeFile::clickedMinimizeButton()
{
	MessageChangeFileView(
		m_filePath,
		MessageChangeFileView::FILE_MINIMIZED,
		MessageChangeFileView::VIEW_LIST,
		false,
		m_navigator->hasErrors()
	).dispatch();
}

void QtCodeFile::clickedSnippetButton()
{
	m_navigator->requestScroll(m_filePath, 0, 0, false, QtCodeNavigateable::SCROLL_VISIBLE);

	MessageChangeFileView(
		m_filePath,
		MessageChangeFileView::FILE_SNIPPETS,
		MessageChangeFileView::VIEW_LIST,
		isCollapsed(),
		m_navigator->hasErrors()
	).dispatch();
}

void QtCodeFile::clickedMaximizeButton()
{
	uint lineNumber = 0;
	Id locationId = 0;

	std::pair<QtCodeSnippet*, Id> snippet = getFirstSnippetWithActiveLocationId(0);
	if (snippet.first)
	{
		locationId = snippet.second;
	}
	else
	{
		lineNumber = 1;

		std::vector<QtCodeSnippet*> snippets = getVisibleSnippets();
		if (snippets.size())
		{
			lineNumber = snippets[0]->getStartLineNumber();
		}
	}

	m_navigator->requestScroll(m_filePath, lineNumber, locationId, false, QtCodeNavigateable::SCROLL_CENTER);

	MessageChangeFileView(
		m_filePath,
		MessageChangeFileView::FILE_MAXIMIZED,
		MessageChangeFileView::VIEW_SINGLE,
		true, // TODO: check if data is really needed
		m_navigator->hasErrors(),
		true
	).dispatch();
}

void QtCodeFile::updateRefCount(int refCount)
{
	if (m_isWholeFile)
	{
		refCount = 0;
	}

	bool hasErrors = m_navigator->hasErrors();
	size_t fatalErrorCount = hasErrors ? m_navigator->getFatalErrorCountForFile(m_filePath) : 0;

	m_titleBar->updateRefCount(refCount, hasErrors, fatalErrorCount);
}
