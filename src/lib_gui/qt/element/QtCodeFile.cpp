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

std::string QtCodeFile::getFileName() const
{
	return m_filePath.fileName();
}

const QtCodeFileTitleBar* QtCodeFile::getTitleBar() const
{
	return m_titleBar;
}

QtCodeSnippet* QtCodeFile::addCodeSnippet(const CodeSnippetParams& params)
{
	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		if (snippet->getStartLineNumber() == params.startLineNumber &&
			snippet->getEndLineNumber() == params.endLineNumber)
		{
			return snippet.get();
		}
	}

	std::shared_ptr<QtCodeSnippet> snippet(new QtCodeSnippet(params, m_navigator, this));

	if (params.reduced)
	{
		m_titleBar->getTitleButton()->setProject(params.title);
		m_isWholeFile = true;
	}

	m_snippetLayout->addWidget(snippet.get());

	if (params.locationFile->isWhole() || m_isWholeFile)
	{
		snippet->setStyleSheet("#code_snippet { border: none; }");

		m_fileSnippet = snippet;
		if (!m_snippets.size())
		{
			m_fileSnippet->setIsActiveFile(true);
		}

		setSnippets();
		if (params.refCount != -1)
		{
			updateRefCount(0);
		}

		return m_fileSnippet.get();
	}

	m_snippets.push_back(snippet);

	setSnippets();
	updateRefCount(params.refCount);

	return snippet.get();
}

QtCodeSnippet* QtCodeFile::insertCodeSnippet(const CodeSnippetParams& params)
{
	std::shared_ptr<QtCodeSnippet> snippet(new QtCodeSnippet(params, m_navigator, this));

	size_t i = 0;
	while (i < m_snippets.size())
	{
		uint start = snippet->getStartLineNumber();
		uint end = snippet->getEndLineNumber();

		std::shared_ptr<QtCodeSnippet> s = m_snippets[i];

		if (s->getEndLineNumber() + 1 < start)
		{
			i++;
			continue;
		}
		else if (s->getStartLineNumber() > end + 1)
		{
			break;
		}
		else if (s->getStartLineNumber() < start || s->getEndLineNumber() > end)
		{
			m_navigator->clearSnippetReferences();
			snippet = QtCodeSnippet::merged(snippet.get(), s.get(), m_navigator, this);
		}

		s->hide();
		m_snippetLayout->removeWidget(s.get());

		m_snippets.erase(m_snippets.begin() + i);
	}

	m_snippetLayout->insertWidget(i, snippet.get());
	m_snippets.insert(m_snippets.begin() + i, snippet);

	setSnippets();

	return snippet.get();
}

std::vector<QtCodeSnippet*> QtCodeFile::getVisibleSnippets() const
{
	if (m_fileSnippet && m_fileSnippet->isVisible())
	{
		return { m_fileSnippet.get() };
	}

	std::vector<QtCodeSnippet*> snippets;

	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		if (snippet->isVisible())
		{
			snippets.push_back(snippet.get());
		}
	}

	return snippets;
}

QtCodeSnippet* QtCodeFile::getSnippetForLocationId(Id locationId) const
{
	if (m_fileSnippet && m_fileSnippet->isVisible() && m_fileSnippet->getLineNumberForLocationId(locationId))
	{
		return m_fileSnippet.get();
	}

	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		if (snippet->getLineNumberForLocationId(locationId))
		{
			return snippet.get();
		}
	}

	return nullptr;
}

QtCodeSnippet* QtCodeFile::getSnippetForLine(unsigned int line) const
{
	if (m_fileSnippet && m_fileSnippet->isVisible())
	{
		return m_fileSnippet.get();
	}

	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		if (snippet->getStartLineNumber() <= line && line <= snippet->getEndLineNumber())
		{
			return snippet.get();
		}
	}

	return nullptr;
}

QtCodeSnippet* QtCodeFile::getFileSnippet() const
{
	return m_fileSnippet.get();
}

std::pair<QtCodeSnippet*, Id> QtCodeFile::getFirstSnippetWithActiveLocationId(Id tokenId) const
{
	std::pair<QtCodeSnippet*, Id> result(nullptr, 0);

	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		Id locationId = snippet->getFirstActiveLocationId(tokenId);
		if (locationId != 0)
		{
			result.first = snippet.get();
			result.second = locationId;
			break;
		}
	}

	return result;
}

bool QtCodeFile::isCollapsed() const
{
	return !getFileSnippet() && !m_snippets.size();
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

	bool needsData = (state == MessageChangeFileView::FILE_MAXIMIZED) ? (getFileSnippet() == nullptr) : (m_snippets.size() == 0);

	MessageChangeFileView(m_filePath, state, MessageChangeFileView::VIEW_LIST, needsData, m_navigator->hasErrors()).dispatch();
}

void QtCodeFile::requestWholeFileContent()
{
	if (!getFileSnippet())
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

	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		snippet->updateContent();
	}

	if (m_fileSnippet)
	{
		m_fileSnippet->updateContent();
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
	m_titleBar->getTitleButton()->setIsComplete(isComplete);
}

void QtCodeFile::setMinimized()
{
	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		snippet->hide();
	}

	if (m_fileSnippet)
	{
		m_fileSnippet->hide();
	}

	m_titleBar->setMinimized();

	setStyleSheet("#code_file { padding-bottom: 0; } #code_file #title_bar { border-radius: 7px; }");
}

void QtCodeFile::setSnippets()
{
	if (m_fileSnippet)
	{
		m_fileSnippet->show();

		for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
		{
			snippet->hide();
		}
	}
	else
	{
		for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
		{
			snippet->show();
		}
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
	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		if (snippet != m_snippets.front() && snippet->styleSheet().size())
		{
			snippet->setStyleSheet("");
		}

		maxDigits = qMax(maxDigits, snippet->lineNumberDigits());
	}

	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
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

void QtCodeFile::findScreenMatches(const std::string& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches)
{
	if (m_fileSnippet && m_fileSnippet->isVisible())
	{
		m_fileSnippet->findScreenMatches(query, screenMatches);
		return;
	}

	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		if (snippet->isVisible())
		{
			snippet->findScreenMatches(query, screenMatches);
		}
	}
}

void QtCodeFile::clickedMinimizeButton()
{
	m_navigator->requestScroll(m_filePath, 0, 0, false, QtCodeNavigateable::SCROLL_VISIBLE);

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
	uint firstLineNumber = 1;
	std::vector<QtCodeSnippet*> snippets = getVisibleSnippets();
	if (snippets.size())
	{
		firstLineNumber = snippets[0]->getStartLineNumber();
	}
	m_navigator->requestScroll(m_filePath, firstLineNumber, 0, false, QtCodeNavigateable::SCROLL_CENTER);

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
