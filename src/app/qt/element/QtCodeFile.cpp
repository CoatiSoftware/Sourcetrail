#include "qt/element/QtCodeFile.h"

#include <QPushButton>
#include <QVBoxLayout>

#include "utility/messaging/type/MessageActivateFile.h"
#include "utility/messaging/type/MessageShowFile.h"

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFileList.h"
#include "qt/element/QtCodeSnippet.h"

QtCodeFile::QtCodeFile(const FilePath& filePath, QtCodeFileList* parent)
	: QFrame(parent)
	, m_parent(parent)
	, m_filePath(filePath)
{
	setObjectName("code_file");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	QFrame* titleWidget = new QFrame(this);
	titleWidget->setObjectName("title_widget");
	layout->addWidget(titleWidget);

	QHBoxLayout* titleLayout = new QHBoxLayout();
	titleLayout->setMargin(0);
	titleLayout->setSpacing(0);
	titleLayout->setAlignment(Qt::AlignLeft);
	titleWidget->setLayout(titleLayout);

	m_title = new QPushButton(filePath.fileName().c_str(), this);
	m_title->setObjectName("title_label");
	m_title->minimumSizeHint(); // force font loading
	m_title->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_title->setToolTip(QString::fromStdString(filePath.str()));
	m_title->setFixedWidth(m_title->fontMetrics().width(filePath.fileName().c_str()) + 32);
	m_title->setFixedHeight(std::max(m_title->fontMetrics().height() * 1.2, 28.0));
	m_title->setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	titleLayout->addWidget(m_title);

	titleWidget->setMinimumHeight(m_title->height() + 4);

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

const FilePath& QtCodeFile::getFilePath() const
{
	return m_filePath;
}

std::string QtCodeFile::getFileName() const
{
	return m_filePath.fileName();
}

Id QtCodeFile::getFocusedTokenId() const
{
	return m_parent->getFocusedTokenId();
}

const std::vector<Id>& QtCodeFile::getActiveTokenIds() const
{
	return m_parent->getActiveTokenIds();
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
	std::shared_ptr<TokenLocationFile> locationFile
){
	std::shared_ptr<QtCodeSnippet> snippet(
		new QtCodeSnippet(startLineNumber, title, titleId, code, locationFile, this));

	m_snippetLayout->addWidget(snippet.get());

	if (locationFile->isWholeCopy)
	{
		snippet->setProperty("isFirst", true);
		snippet->setProperty("isLast", true);
		m_fileSnippet = snippet;
		clickedMaximizeButton();
		return;
	}

	m_snippets.push_back(snippet);

	updateSnippets();
}

QWidget* QtCodeFile::insertCodeSnippet(
	uint startLineNumber,
	const std::string& title,
	Id titleId,
	const std::string& code,
	std::shared_ptr<TokenLocationFile> locationFile
){
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

	return snippet.get();
}

QWidget* QtCodeFile::findFirstActiveSnippet() const
{
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		if (snippet->isActive())
		{
			return snippet.get();
		}
	}

	return nullptr;
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
	if (m_snippets.size())
	{
		m_snippetButton->setEnabled(true);
	}
	m_maximizeButton->setEnabled(true);

	m_minimizePlaceholder->show();
}

void QtCodeFile::clickedSnippetButton()
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
		MessageShowFile(m_filePath.str(), 0, 0).dispatch();
	}

	m_minimizeButton->setEnabled(true);
	if (m_snippets.size())
	{
		m_snippetButton->setEnabled(true);
	}
	m_maximizeButton->setEnabled(false);

	m_minimizePlaceholder->hide();
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
