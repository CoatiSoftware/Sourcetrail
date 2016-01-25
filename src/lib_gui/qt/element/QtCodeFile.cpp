#include "qt/element/QtCodeFile.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageActivateFile.h"
#include "utility/messaging/type/MessageShowFile.h"
#include "utility/messaging/type/MessageShowSnippets.h"

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFileList.h"
#include "qt/element/QtCodeSnippet.h"
#include "qt/utility/utilityQt.h"
#include "settings/ColorScheme.h"

QtCodeFile::QtCodeFile(const FilePath& filePath, QtCodeFileList* parent)
	: QFrame(parent)
	, m_updateTitleBarFunctor(std::bind(&QtCodeFile::doUpdateTitleBar, this))
	, m_parent(parent)
	, m_filePath(filePath)
{
	setObjectName("code_file");

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
	m_title->setFixedWidth(m_title->fontMetrics().width(filePath.fileName().c_str()) + 52);
	m_title->setFixedHeight(std::max(m_title->fontMetrics().height() * 1.2, 28.0));
	m_title->setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);

	m_title->setIcon(utility::colorizePixmap(
		QPixmap("data/gui/graph_view/images/file.png"),
		ColorScheme::getInstance()->getColor("code/file/title/icon").c_str()
	));

	titleLayout->addWidget(m_title);

	if (m_title->text().size() == 0)
	{
		m_title->hide();
	}

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

	m_minimizePlaceholder = new QWidget(this);
	m_minimizePlaceholder->setMinimumHeight(5);
	layout->addWidget(m_minimizePlaceholder);

	m_snippetLayout = new QVBoxLayout();
	layout->addLayout(m_snippetLayout);

	update();
}

QtCodeFile::~QtCodeFile()
{
}

void QtCodeFile::setModificationTime(TimePoint modificationTime)
{
	m_modificationTime = modificationTime;
	updateTitleBar();
}

const FilePath& QtCodeFile::getFilePath() const
{
	return m_filePath;
}

std::string QtCodeFile::getFileName() const
{
	return m_filePath.fileName();
}

const std::vector<Id>& QtCodeFile::getActiveTokenIds() const
{
	return m_parent->getActiveTokenIds();
}

const std::vector<Id>& QtCodeFile::getFocusedTokenIds() const
{
	return m_parent->getFocusedTokenIds();
}

const std::vector<std::string>& QtCodeFile::getErrorMessages() const
{
	return m_parent->getErrorMessages();
}

void QtCodeFile::addCodeSnippet(
	uint startLineNumber,
	const std::string& title,
	Id titleId,
	const std::string& code,
	std::shared_ptr<TokenLocationFile> locationFile,
	int refCount
){
	m_locationFile.reset();

	std::shared_ptr<QtCodeSnippet> snippet(
		new QtCodeSnippet(startLineNumber, title, titleId, code, locationFile, this));

	m_snippetLayout->addWidget(snippet.get());

	if (locationFile->isWholeCopy)
	{
		snippet->setProperty("isFirst", true);
		snippet->setProperty("isLast", true);

		m_fileSnippet = snippet;
		if (!m_snippets.size())
		{
			m_fileSnippet->setIsActiveFile(true);
		}

		clickedMaximizeButton();
		if (refCount != -1)
		{
			updateRefCount(0);
		}
		return;
	}

	m_snippets.push_back(snippet);

	updateSnippets();
	updateRefCount(refCount);
}

QtCodeSnippet* QtCodeFile::insertCodeSnippet(
	uint startLineNumber,
	const std::string& title,
	Id titleId,
	const std::string& code,
	std::shared_ptr<TokenLocationFile> locationFile,
	int refCount
){
	m_locationFile.reset();

	std::shared_ptr<QtCodeSnippet> snippet(
		new QtCodeSnippet(startLineNumber, title, titleId, code, locationFile, this));

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
			snippet = QtCodeSnippet::merged(snippet.get(), s.get(), this);
		}

		s->hide();
		m_snippetLayout->removeWidget(s.get());

		m_snippets.erase(m_snippets.begin() + i);
	}

	m_snippetLayout->insertWidget(i, snippet.get());
	m_snippets.insert(m_snippets.begin() + i, snippet);

	updateSnippets();
	updateRefCount(refCount);

	return snippet.get();
}

QtCodeSnippet* QtCodeFile::findFirstActiveSnippet() const
{
	if (m_locationFile)
	{
		return nullptr;
	}

	if (m_maximizeButton->isEnabled())
	{
		for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
		{
			if (snippet->isActive())
			{
				return snippet.get();
			}
		}
	}
	else
	{
		if (m_fileSnippet->isActive())
		{
			return m_fileSnippet.get();
		}
	}

	return nullptr;
}

bool QtCodeFile::openCollapsedActiveSnippet() const
{
	if (m_locationFile)
	{
		std::vector<Id> ids = getActiveTokenIds();

		bool isActiveFile = false;
		m_locationFile->forEachTokenLocation(
			[&](TokenLocation* location)
			{
				if (std::find(ids.begin(), ids.end(), location->getTokenId()) != ids.end())
				{
					isActiveFile = true;
				}
			}
		);

		if (isActiveFile)
		{
			MessageShowSnippets(m_locationFile).dispatch();
			return true;
		}
	}

	return false;
}

void QtCodeFile::updateContent()
{
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
	clickedMinimizeButton();

	updateRefCount(refCount);
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

void QtCodeFile::clickedMinimizeButton()
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
	if (m_snippets.size() || m_locationFile)
	{
		m_snippetButton->setEnabled(true);
	}
	m_maximizeButton->setEnabled(true);

	m_minimizePlaceholder->show();
}

void QtCodeFile::clickedSnippetButton()
{
	if (m_locationFile)
	{
		MessageShowSnippets(m_locationFile).dispatch();
		return;
	}

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

	m_minimizePlaceholder->hide();
}

void QtCodeFile::clickedMaximizeButton()
{
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		snippet->hide();
	}

	if (m_fileSnippet)
	{
		m_fileSnippet->show();
	}
	else
	{
		MessageShowFile(m_filePath, (getErrorMessages().size() > 0)).dispatch();
	}

	m_minimizeButton->setEnabled(true);
	if (m_snippets.size())
	{
		m_snippetButton->setEnabled(true);
	}
	m_maximizeButton->setEnabled(false);

	m_minimizePlaceholder->hide();
}

void QtCodeFile::handleMessage(MessageWindowFocus* message)
{
	updateTitleBar();
}

void QtCodeFile::updateSnippets()
{
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

	clickedSnippetButton();
}

void QtCodeFile::updateRefCount(int refCount)
{
	if (refCount > 0)
	{
		m_referenceCount->setText(QString::fromStdString(std::to_string(refCount) + (refCount == 1 ? " reference" : " references")));
		m_referenceCount->show();
	}
	else
	{
		m_referenceCount->hide();
	}
}

void QtCodeFile::updateTitleBar()
{
	m_updateTitleBarFunctor();
}

void QtCodeFile::doUpdateTitleBar()
{
	// cannot use m_filePath.exists() here since it is only checked when FilePath is constructed.
	if ((!FileSystem::exists(m_filePath.str())) ||
		(FileSystem::getLastWriteTime(m_filePath) > m_modificationTime))
	{
		m_title->setStyleSheet("background-image: url(data/gui/code_view/images/pattern.png);");
	}
	else
	{
		m_title->setStyleSheet("");
	}
}
