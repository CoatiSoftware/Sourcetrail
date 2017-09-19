#include "qt/element/QtCodeFile.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "utility/messaging/type/MessageChangeFileView.h"
#include "utility/ResourcePaths.h"

#include "data/location/SourceLocationFile.h"
#include "qt/element/QtCodeFileTitleButton.h"
#include "qt/element/QtCodeNavigator.h"
#include "qt/element/QtCodeSnippet.h"

QtCodeFile::QtCodeFile(const FilePath& filePath, QtCodeNavigator* navigator)
	: QFrame()
	, m_navigator(navigator)
	, m_filePath(filePath)
	, m_isWholeFile(false)
	, m_isCollapsed(true)
	, m_contentRequested(false)
{
	setObjectName("code_file");
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_titleBar = new QtHoverButton(this);
	m_titleBar->setObjectName("title_widget");
	m_titleBar->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	layout->addWidget(m_titleBar);

	connect(dynamic_cast<QtHoverButton*>(m_titleBar), &QtHoverButton::hoveredIn, this, &QtCodeFile::enteredTitleBar);
	connect(dynamic_cast<QtHoverButton*>(m_titleBar), &QtHoverButton::hoveredOut, this, &QtCodeFile::leftTitleBar);

	QHBoxLayout* titleLayout = new QHBoxLayout();
	titleLayout->setMargin(0);
	titleLayout->setSpacing(0);
	titleLayout->setAlignment(Qt::AlignLeft);
	m_titleBar->setLayout(titleLayout);

	m_title = new QtCodeFileTitleButton(this);
	if (!m_filePath.empty())
	{
		m_title->setFilePath(filePath);
	}

	titleLayout->addWidget(m_title);

	m_titleBar->setMinimumHeight(m_title->height() + 4);

	m_referenceCount = new QLabel(this);
	m_referenceCount->setObjectName("references_label");
	m_referenceCount->hide();
	titleLayout->addWidget(m_referenceCount);

	titleLayout->addStretch(3);

	m_minimizeButton = new QtIconStateButton(this);
	m_minimizeButton->addState(QtIconStateButton::STATE_DEFAULT, (ResourcePaths::getGuiPath().str() + "code_view/images/minimize_active.png").c_str());
//	m_minimizeButton->addState(QtIconStateButton::STATE_HOVERED, (ResourcePaths::getGuiPath().str() + "code_view/images/minimize_inactive.png").c_str(), "#5E5D5D");
	m_minimizeButton->addState(QtIconStateButton::STATE_HOVERED, (ResourcePaths::getGuiPath().str() + "code_view/images/minimize_inactive.png").c_str(), QColor(0x5E, 0x5D, 0x5D));
	m_minimizeButton->addState(QtIconStateButton::STATE_DISABLED, (ResourcePaths::getGuiPath().str() + "code_view/images/minimize_inactive.png").c_str());
	m_minimizeButton->setObjectName("file_button");
	m_minimizeButton->setToolTip("minimize");
	titleLayout->addWidget(m_minimizeButton);

	m_snippetButton = new QtIconStateButton(this);
	m_snippetButton->addState(QtIconStateButton::STATE_DEFAULT, (ResourcePaths::getGuiPath().str() + "code_view/images/snippet_active.png").c_str());
//	m_snippetButton->addState(QtIconStateButton::STATE_HOVERED, (ResourcePaths::getGuiPath().str() + "code_view/images/snippet_inactive.png").c_str(), "#5E5D5D");
	m_snippetButton->addState(QtIconStateButton::STATE_HOVERED, (ResourcePaths::getGuiPath().str() + "code_view/images/snippet_inactive.png").c_str(), QColor(0x5E, 0x5D, 0x5D));
	m_snippetButton->addState(QtIconStateButton::STATE_DISABLED, (ResourcePaths::getGuiPath().str() + "code_view/images/snippet_inactive.png").c_str());
	m_snippetButton->setObjectName("file_button");
	m_snippetButton->setToolTip("show snippets");
	titleLayout->addWidget(m_snippetButton);

	m_maximizeButton = new QtIconStateButton(this);
	m_maximizeButton->addState(QtIconStateButton::STATE_DEFAULT, (ResourcePaths::getGuiPath().str() + "code_view/images/maximize_active.png").c_str());
//	m_maximizeButton->addState(QtIconStateButton::STATE_HOVERED, (ResourcePaths::getGuiPath().str() + "code_view/images/maximize_inactive.png").c_str(), "#5E5D5D");
	m_maximizeButton->addState(QtIconStateButton::STATE_HOVERED, (ResourcePaths::getGuiPath().str() + "code_view/images/maximize_inactive.png").c_str(), QColor(0x5E, 0x5D, 0x5D));
	m_maximizeButton->addState(QtIconStateButton::STATE_DISABLED, (ResourcePaths::getGuiPath().str() + "code_view/images/maximize_inactive.png").c_str());
	m_maximizeButton->setObjectName("file_button");
	m_maximizeButton->setToolTip("maximize");
	titleLayout->addWidget(m_maximizeButton);

	connect(m_minimizeButton, &QtIconStateButton::hoveredIn, this, &QtCodeFile::leftTitleBar);
	connect(m_minimizeButton, &QtIconStateButton::hoveredOut, this, &QtCodeFile::enteredTitleBar);
	connect(m_snippetButton, &QtIconStateButton::hoveredIn, this, &QtCodeFile::leftTitleBar);
	connect(m_snippetButton, &QtIconStateButton::hoveredOut, this, &QtCodeFile::enteredTitleBar);
	connect(m_maximizeButton, &QtIconStateButton::hoveredIn, this, &QtCodeFile::leftTitleBar);
	connect(m_maximizeButton, &QtIconStateButton::hoveredOut, this, &QtCodeFile::enteredTitleBar);

	titleLayout->addSpacing(3);

	m_minimizeButton->setEnabled(false);
	m_snippetButton->setEnabled(false);
	m_maximizeButton->setEnabled(false);

	connect(m_titleBar, &QPushButton::clicked, this, &QtCodeFile::clickedTitleBar);
	connect(m_minimizeButton, &QtIconStateButton::clicked, this, &QtCodeFile::clickedMinimizeButton);
	connect(m_snippetButton, &QtIconStateButton::clicked, this, &QtCodeFile::clickedSnippetButton);
	connect(m_maximizeButton, &QtIconStateButton::clicked, this, &QtCodeFile::clickedMaximizeButton);

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
	m_title->setModificationTime(modificationTime);
}

const FilePath& QtCodeFile::getFilePath() const
{
	return m_filePath;
}

std::string QtCodeFile::getFileName() const
{
	return m_filePath.fileName();
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

	m_isCollapsed = false;
	std::shared_ptr<QtCodeSnippet> snippet(new QtCodeSnippet(params, m_navigator, this));

	if (params.reduced)
	{
		m_title->setProject(params.title);
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

		setMaximized();
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
	m_isCollapsed = false;

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
	return m_isCollapsed;
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

	MessageChangeFileView(m_filePath, state, isCollapsed(), m_navigator->hasErrors()).dispatch();
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
	m_title->setIsComplete(isComplete);
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

	m_minimizeButton->setEnabled(false);
	m_snippetButton->setEnabled(m_snippets.size() || (isCollapsed() && !m_isWholeFile));
	m_maximizeButton->setEnabled(true);

	m_minimizeButton->hoverOut();
	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();

	setStyleSheet("#code_file { padding-bottom: 0; } #code_file #title_widget { border-radius: 7px; }");
}

void QtCodeFile::setSnippets()
{
	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		snippet->show();
	}

	if (m_fileSnippet)
	{
		m_fileSnippet->hide();
	}

	m_minimizeButton->setEnabled(true);
	m_snippetButton->setEnabled(false);
	m_maximizeButton->setEnabled(true);

	m_minimizeButton->hoverOut();
	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();

	setStyleSheet("");
}

void QtCodeFile::setMaximized()
{
	for (const std::shared_ptr<QtCodeSnippet>& snippet : m_snippets)
	{
		snippet->hide();
	}

	if (m_fileSnippet)
	{
		m_fileSnippet->show();
	}

	m_minimizeButton->setEnabled(true);
	m_snippetButton->setEnabled(m_snippets.size() || (isCollapsed() && !m_isWholeFile));
	m_maximizeButton->setEnabled(false);

	m_minimizeButton->hoverOut();
	m_snippetButton->hoverOut();
	m_maximizeButton->hoverOut();

	setStyleSheet("");
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
	m_title->updateTexts();
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
	// overview stats
	if (m_filePath.empty())
	{
		setMinimized();
		return;
	}

	MessageChangeFileView(
		m_filePath,
		MessageChangeFileView::FILE_MINIMIZED,
		false,
		m_navigator->hasErrors()
	).dispatch();
}

void QtCodeFile::clickedSnippetButton()
{
	MessageChangeFileView(
		m_filePath,
		MessageChangeFileView::FILE_SNIPPETS,
		isCollapsed(),
		m_navigator->hasErrors()
	).dispatch();
}

void QtCodeFile::clickedMaximizeButton()
{
	// overview stats
	if (m_filePath.empty())
	{
		setMaximized();
		return;
	}

	MessageChangeFileView(
		m_filePath,
		MessageChangeFileView::FILE_MAXIMIZED,
		!getFileSnippet(),
		m_navigator->hasErrors()
	).dispatch();
}

void QtCodeFile::enteredTitleBar(QPushButton* button)
{
	if (m_minimizeButton->isEnabled())
	{
		m_minimizeButton->hoverIn();
	}
	else if (m_snippetButton->isEnabled())
	{
		m_snippetButton->hoverIn();
	}
	else if (m_maximizeButton->isEnabled())
	{
		m_maximizeButton->hoverIn();
	}
}

void QtCodeFile::leftTitleBar(QPushButton* button)
{
	if (m_minimizeButton->isEnabled())
	{
		if (m_minimizeButton != button)
		{
			m_minimizeButton->hoverOut();
		}
	}
	else if (m_snippetButton->isEnabled())
	{
		if (m_snippetButton != button)
		{
			m_snippetButton->hoverOut();
		}
	}
	else if (m_maximizeButton->isEnabled())
	{
		if (m_maximizeButton != button)
		{
			m_maximizeButton->hoverOut();
		}
	}
}

void QtCodeFile::clickedTitleBar()
{
	if (m_minimizeButton->isEnabled())
	{
		clickedMinimizeButton();
	}
	else if (m_snippetButton->isEnabled())
	{
		clickedSnippetButton();
	}
	else
	{
		clickedMaximizeButton();
	}
}

void QtCodeFile::updateRefCount(int refCount)
{
	if (refCount > 0 && !m_isWholeFile)
	{
		bool hasErrors = m_navigator->hasErrors();

		QString label = hasErrors ? "error" : "reference";
		if (refCount > 1)
		{
			label += "s";
		}

		if (hasErrors)
		{
			size_t fatalErrorCount = m_navigator->getFatalErrorCountForFile(m_filePath);
			if (fatalErrorCount > 0)
			{
				label += " (" + QString::number(fatalErrorCount) + " fatal)";
			}
		}

		m_referenceCount->setText(QString::number(refCount) + " " + label);
		m_referenceCount->show();
	}
	else
	{
		m_referenceCount->hide();
	}
}
