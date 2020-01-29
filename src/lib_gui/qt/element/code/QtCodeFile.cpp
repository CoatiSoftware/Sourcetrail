#include "QtCodeFile.h"

#include <QStyle>
#include <QVBoxLayout>

#include "MessageChangeFileView.h"

#include "QtCodeFileTitleBar.h"
#include "QtCodeNavigator.h"
#include "QtCodeSnippet.h"
#include "SourceLocationFile.h"

QtCodeFile::QtCodeFile(const FilePath& filePath, QtCodeNavigator* navigator, bool isFirst)
	: QFrame(), m_navigator(navigator), m_filePath(filePath), m_isWholeFile(false)
{
	setObjectName(QStringLiteral("code_file"));
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_titleBar = new QtCodeFileTitleBar(this, isFirst);
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

	setMinimized();
	update();
}

QtCodeFile::~QtCodeFile() {}

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
	QtCodeSnippet* snippet = new QtCodeSnippet(params, m_navigator, this);

	if (params.isOverview)
	{
		m_titleBar->getTitleButton()->setProject(params.title);
	}

	if (params.locationFile->isWhole() || m_isWholeFile)
	{
		m_isWholeFile = true;
		snippet->setIsActiveFile(true);
	}

	m_snippetLayout->addWidget(snippet);
	m_snippets.push_back(snippet);

	return snippet;
}

void QtCodeFile::updateSourceLocations(const CodeSnippetParams& params)
{
	if (m_isWholeFile && m_snippets.size() == 1)
	{
		m_snippets[0]->updateSourceLocations(params);
		return;
	}

	for (QtCodeSnippet* snippet: m_snippets)
	{
		if (snippet->getStartLineNumber() == params.startLineNumber &&
			snippet->getEndLineNumber() == params.endLineNumber)
		{
			snippet->updateSourceLocations(params);
		}
	}
}

const std::vector<QtCodeSnippet*>& QtCodeFile::getSnippets() const
{
	return m_snippets;
}

std::vector<QtCodeSnippet*> QtCodeFile::getVisibleSnippets() const
{
	std::vector<QtCodeSnippet*> snippets;

	for (QtCodeSnippet* snippet: m_snippets)
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
	for (QtCodeSnippet* snippet: m_snippets)
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
	for (QtCodeSnippet* snippet: m_snippets)
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

	for (QtCodeSnippet* snippet: m_snippets)
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

void QtCodeFile::requestWholeFileContent(size_t targetLineNumber)
{
	if (!m_isWholeFile)
	{
		MessageChangeFileView msg(
			m_filePath,
			MessageChangeFileView::FILE_MAXIMIZED,
			MessageChangeFileView::VIEW_LIST,
			CodeScrollParams::toLine(m_filePath, targetLineNumber, CodeScrollParams::Target::VISIBLE));
		msg.setSchedulerId(m_navigator->getSchedulerId());
		msg.dispatch();
	}
	else
	{
		setSnippets();
	}
}

void QtCodeFile::updateContent()
{
	updateSnippets();

	for (QtCodeSnippet* snippet: m_snippets)
	{
		snippet->updateContent();
	}
}

void QtCodeFile::setWholeFile(bool isWholeFile, int refCount)
{
	m_isWholeFile = isWholeFile;

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
	for (QtCodeSnippet* snippet: m_snippets)
	{
		snippet->hide();
	}

	m_titleBar->setMinimized();
}

void QtCodeFile::setSnippets()
{
	for (QtCodeSnippet* snippet: m_snippets)
	{
		snippet->show();
	}

	m_titleBar->setSnippets();
}

bool QtCodeFile::hasSnippets() const
{
	return m_snippets.size() > 0;
}

void QtCodeFile::clearSnippets()
{
	for (QtCodeSnippet* snippet: m_snippets)
	{
		m_snippetLayout->removeWidget(snippet);
		snippet->hide();
		snippet->deleteLater();
	}

	if (m_snippets.size())
	{
		m_navigator->clearSnippetReferences();
	}

	m_snippets.clear();
}

void QtCodeFile::updateSnippets()
{
	if (m_snippets.size() == 0)
	{
		return;
	}

	int maxDigits = 1;
	for (QtCodeSnippet* snippet: m_snippets)
	{
		maxDigits = qMax(maxDigits, snippet->lineNumberDigits());
	}

	for (QtCodeSnippet* snippet: m_snippets)
	{
		snippet->setProperty("first", snippet == m_snippets.front());
		snippet->style()->polish(snippet);	  // recomputes style to make property take effect
		snippet->updateLineNumberAreaWidthForDigits(maxDigits);
	}
}

void QtCodeFile::updateTitleBar()
{
	m_titleBar->getTitleButton()->updateTexts();
}

void QtCodeFile::findScreenMatches(
	const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches)
{
	for (QtCodeSnippet* snippet: m_snippets)
	{
		if (snippet->isVisible())
		{
			snippet->findScreenMatches(query, screenMatches);
		}
	}
}

void QtCodeFile::clickedMinimizeButton()
{
	MessageChangeFileView msg(
		m_filePath,
		MessageChangeFileView::FILE_MINIMIZED,
		MessageChangeFileView::VIEW_LIST,
		CodeScrollParams::toFile(m_filePath, CodeScrollParams::Target::VISIBLE));
	msg.setSchedulerId(m_navigator->getSchedulerId());
	msg.dispatch();
}

void QtCodeFile::clickedSnippetButton()
{
	MessageChangeFileView msg(
		m_filePath,
		MessageChangeFileView::FILE_SNIPPETS,
		MessageChangeFileView::VIEW_LIST,
		CodeScrollParams::toFile(m_filePath, CodeScrollParams::Target::VISIBLE));
	msg.setSchedulerId(m_navigator->getSchedulerId());
	msg.dispatch();
}

void QtCodeFile::clickedMaximizeButton()
{
	size_t lineNumber = 0;
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

	m_navigator->setMode(QtCodeNavigator::MODE_SINGLE);

	CodeScrollParams scrollParams = locationId
		? CodeScrollParams::toReference(m_filePath, locationId, CodeScrollParams::Target::CENTER)
		: CodeScrollParams::toLine(m_filePath, lineNumber, CodeScrollParams::Target::CENTER);

	MessageChangeFileView msg(
		m_filePath,
		MessageChangeFileView::FILE_MAXIMIZED,
		MessageChangeFileView::VIEW_SINGLE,
		scrollParams,
		true);
	msg.setSchedulerId(m_navigator->getSchedulerId());
	msg.dispatch();
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
