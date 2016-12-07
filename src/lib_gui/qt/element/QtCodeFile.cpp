#include "qt/element/QtCodeFile.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "utility/ResourcePaths.h"
#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageActivateFile.h"
#include "utility/messaging/type/MessageChangeFileView.h"
#include "utility/messaging/type/MessageProjectEdit.h"

#include "Application.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeNavigator.h"
#include "qt/element/QtCodeSnippet.h"
#include "qt/utility/utilityQt.h"
#include "settings/ColorScheme.h"

QtCodeFile::QtCodeFile(const FilePath& filePath, QtCodeNavigator* navigator)
	: QFrame()
	, m_navigator(navigator)
	, m_filePath(filePath)
	, m_contentRequested(false)
{
	setObjectName("code_file");
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_titleBar = new QPushButton(this);
	m_titleBar->setObjectName("title_widget");
	layout->addWidget(m_titleBar);

	QHBoxLayout* titleLayout = new QHBoxLayout();
	titleLayout->setMargin(0);
	titleLayout->setSpacing(0);
	titleLayout->setAlignment(Qt::AlignLeft);
	m_titleBar->setLayout(titleLayout);

	m_title = new QPushButton(filePath.fileName().c_str(), this);
	m_title->setObjectName("title_label");
	m_title->minimumSizeHint(); // force font loading
	m_title->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_title->setToolTip(QString::fromStdString(filePath.str()));
	m_title->setFixedHeight(std::max(m_title->fontMetrics().height() * 1.2, 28.0));
	m_title->setSizePolicy(m_title->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);

	std::string text = ResourcePaths::getGuiPath() + "graph_view/images/file.png";

	m_title->setIcon(utility::colorizePixmap(
		QPixmap(text.c_str()),
		ColorScheme::getInstance()->getColor("code/file/title/icon").c_str()
	));

	titleLayout->addWidget(m_title);

	m_titleBar->setMinimumHeight(m_title->height() + 4);

	m_referenceCount = new QLabel(this);
	m_referenceCount->setObjectName("references_label");
	m_referenceCount->hide();
	titleLayout->addWidget(m_referenceCount);

	titleLayout->addStretch(3);

	m_minimizeButton = new QPushButton(this);
	m_minimizeButton->setObjectName("minimize_button");
	m_minimizeButton->setToolTip("minimize");
	m_minimizeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	titleLayout->addWidget(m_minimizeButton);

	m_snippetButton = new QPushButton(this);
	m_snippetButton->setObjectName("snippet_button");
	m_snippetButton->setToolTip("show snippets");
	m_snippetButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	titleLayout->addWidget(m_snippetButton);

	m_maximizeButton = new QPushButton(this);
	m_maximizeButton->setObjectName("maximize_button");
	m_maximizeButton->setToolTip("maximize");
	m_maximizeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	titleLayout->addWidget(m_maximizeButton);

	m_minimizeButton->setEnabled(false);
	m_snippetButton->setEnabled(false);
	m_maximizeButton->setEnabled(false);

	connect(m_titleBar, SIGNAL(clicked()), this, SLOT(clickedTitleBar()));
	connect(m_title, SIGNAL(clicked()), this, SLOT(clickedTitle()));
	connect(m_minimizeButton, SIGNAL(clicked()), this, SLOT(clickedMinimizeButton()));
	connect(m_snippetButton, SIGNAL(clicked()), this, SLOT(clickedSnippetButton()));
	connect(m_maximizeButton, SIGNAL(clicked()), this, SLOT(clickedMaximizeButton()));

	m_snippetLayout = new QVBoxLayout();
	layout->addLayout(m_snippetLayout);

	update();
}

QtCodeFile::~QtCodeFile()
{
}

void QtCodeFile::setModificationTime(TimePoint modificationTime)
{
	if (modificationTime.isValid())
	{
		m_modificationTime = modificationTime;
		updateTitleBar();
	}
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
	m_locationFile.reset();

	std::shared_ptr<QtCodeSnippet> snippet(new QtCodeSnippet(params, m_navigator, this));

	if (params.reduced)
	{
		m_title->hide();

		QPushButton* title = new QPushButton(params.title.c_str(), this);
		title->setObjectName("title_label");
		title->minimumSizeHint(); // force font loading
		title->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
		title->setFixedHeight(std::max(title->fontMetrics().height() * 1.2, 28.0));
		title->setSizePolicy(title->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);

		if (Application::getInstance()->isInTrial())
		{
			title->setEnabled(false);
		}
		else
		{
			std::string text = ResourcePaths::getGuiPath() + "code_view/images/edit.png";
			title->setToolTip("edit project");

			title->setIcon(utility::colorizePixmap(
				QPixmap(text.c_str()),
				ColorScheme::getInstance()->getColor("code/file/title/icon").c_str()
			));

			connect(title, SIGNAL(clicked()), this, SLOT(editProject()));
		}

		dynamic_cast<QHBoxLayout*>(m_titleBar->layout())->insertWidget(1, title);
	}

	m_snippetLayout->addWidget(snippet.get());

	if (params.locationFile->isWholeCopy)
	{
		snippet->setProperty("isFirst", true);
		snippet->setProperty("isLast", true);

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
	m_locationFile.reset();

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
	updateRefCount(params.refCount);

	return snippet.get();
}

QtCodeSnippet* QtCodeFile::getSnippetForLocationId(Id locationId) const
{
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
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

	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
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

std::pair<QtCodeSnippet*, int> QtCodeFile::getFirstSnippetWithActiveScope() const
{
	std::pair<QtCodeSnippet*, int> result(nullptr, 0);

	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		int startLineNumber = snippet->getStartLineNumberOfFirstActiveScope();
		if (startLineNumber != 0)
		{
			result.first = snippet.get();
			result.second = startLineNumber;
			break;
		}
	}

	return result;
}

bool QtCodeFile::isCollapsed() const
{
	return m_locationFile != nullptr;
}

void QtCodeFile::requestContent() const
{
	if (!isCollapsed() || m_contentRequested)
	{
		return;
	}

	m_contentRequested = true;

	MessageChangeFileView msg(
		m_filePath,
		m_locationFile->isWholeCopy ? MessageChangeFileView::FILE_MAXIMIZED : MessageChangeFileView::FILE_SNIPPETS,
		isCollapsed(),
		m_navigator->hasErrors()
	);

	msg.setIsReplayed(true);
	msg.dispatch();
}

void QtCodeFile::updateContent()
{
	updateSnippets();

	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		snippet->updateContent();
	}

	if (m_fileSnippet)
	{
		m_fileSnippet->updateContent();
	}
}

void QtCodeFile::setLocationFile(std::shared_ptr<TokenLocationFile> locationFile, int refCount)
{
	m_locationFile = locationFile;
	setMinimized();

	updateRefCount(locationFile->isWholeCopy ? 0 : refCount);
}

void QtCodeFile::setMinimized()
{
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		snippet->hide();
	}

	if (m_fileSnippet)
	{
		m_fileSnippet->hide();
	}

	m_minimizeButton->setEnabled(false);
	m_snippetButton->setEnabled(m_snippets.size() || (isCollapsed() && !m_locationFile->isWholeCopy));
	m_maximizeButton->setEnabled(true);
}

void QtCodeFile::setSnippets()
{
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
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
}

void QtCodeFile::setMaximized()
{
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		snippet->hide();
	}

	if (m_fileSnippet)
	{
		m_fileSnippet->show();
	}

	m_minimizeButton->setEnabled(true);
	m_snippetButton->setEnabled(m_snippets.size());
	m_maximizeButton->setEnabled(false);
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
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		snippet->setProperty("isFirst", false);
		snippet->setProperty("isLast", false);

		maxDigits = qMax(maxDigits, snippet->lineNumberDigits());
	}

	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		snippet->updateLineNumberAreaWidthForDigits(maxDigits);
	}

	m_snippets.front()->setProperty("isFirst", true);
	m_snippets.back()->setProperty("isLast", true);
}

void QtCodeFile::updateTitleBar()
{
	if (Application::getInstance()->isInTrial())
	{
		return;
	}

	// cannot use m_filePath.exists() here since it is only checked when FilePath is constructed.
	if ((!FileSystem::exists(m_filePath.str())) ||
		(FileSystem::getLastWriteTime(m_filePath) > m_modificationTime))
	{
		m_title->setText(QString(m_filePath.fileName().c_str()) + "*");
		m_title->setToolTip(QString::fromStdString("out of date: " + m_filePath.str()));
	}
	else
	{
		m_title->setText(m_filePath.fileName().c_str());
		m_title->setToolTip(QString::fromStdString(m_filePath.str()));
	}
}

void QtCodeFile::clickedMinimizeButton() const
{
	MessageChangeFileView(
		m_filePath,
		MessageChangeFileView::FILE_MINIMIZED,
		false,
		m_navigator->hasErrors()
	).dispatch();
}

void QtCodeFile::clickedSnippetButton() const
{
	MessageChangeFileView(
		m_filePath,
		MessageChangeFileView::FILE_SNIPPETS,
		isCollapsed(),
		m_navigator->hasErrors()
	).dispatch();
}

void QtCodeFile::clickedMaximizeButton() const
{
	MessageChangeFileView(
		m_filePath,
		MessageChangeFileView::FILE_MAXIMIZED,
		!getFileSnippet(),
		m_navigator->hasErrors()
	).dispatch();
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

void QtCodeFile::clickedTitle()
{
	MessageActivateFile(m_filePath).dispatch();
}

void QtCodeFile::editProject()
{
	MessageProjectEdit().dispatch();
}

void QtCodeFile::updateRefCount(int refCount)
{
	if (refCount > 0)
	{
		QString label = m_navigator->hasErrors() ? "error" : "reference";
		if (refCount > 1)
		{
			label += "s";
		}

		size_t fatalErrorCount = m_navigator->getFatalErrorCountForFile(m_filePath);
		if (fatalErrorCount > 0)
		{
			label += " (" + QString::number(fatalErrorCount) + " fatal)";
		}

		m_referenceCount->setText(QString::number(refCount) + " " + label);
		m_referenceCount->show();
	}
	else
	{
		m_referenceCount->hide();
	}
}
