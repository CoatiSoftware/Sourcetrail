#include "qt/element/QtCodeFile.h"

#include <QPushButton>
#include <QVBoxLayout>

#include "utility/messaging/type/MessageActivateFile.h"
#include "utility/messaging/type/MessageShowFile.h"

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFileList.h"
#include "qt/element/QtCodeSnippet.h"

QtCodeFile::QtCodeFile(const FilePath& filePath, QtCodeFileList* parent)
	: QWidget(parent)
	, m_parent(parent)
	, m_filePath(filePath)
{
	setObjectName("code_file");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	QHBoxLayout* titleLayout = new QHBoxLayout();
	titleLayout->setMargin(0);
	titleLayout->setSpacing(0);
	titleLayout->setAlignment(Qt::AlignLeft);
	layout->addLayout(titleLayout);

	m_title = new QPushButton(filePath.fileName().c_str(), this);
	m_title->setObjectName("title_label");
	m_title->minimumSizeHint(); // force font loading
	m_title->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_title->setToolTip(QString::fromStdString(filePath.str()));
	m_title->setFixedWidth(m_title->fontMetrics().width(filePath.fileName().c_str()) + 32);
	m_title->setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	titleLayout->addWidget(m_title);

	titleLayout->addStretch(3);

	m_minimizeButton = new QPushButton(this);
	m_minimizeButton->setObjectName("maximize_button");
	m_minimizeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	titleLayout->addWidget(m_minimizeButton);

	m_snippetButton = new QPushButton(this);
	m_snippetButton->setObjectName("maximize_button");
	m_snippetButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	titleLayout->addWidget(m_snippetButton);

	m_maximizeButton = new QPushButton(this);
	m_maximizeButton->setObjectName("maximize_button");
	m_maximizeButton->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	titleLayout->addWidget(m_maximizeButton);

	m_minimizeButton->setEnabled(false);
	m_snippetButton->setEnabled(false);
	m_maximizeButton->setEnabled(false);

	connect(m_title, SIGNAL(clicked()), this, SLOT(clickedTitle()));
	connect(m_minimizeButton, SIGNAL(clicked()), this, SLOT(clickedMinimizeButton()));
	connect(m_snippetButton, SIGNAL(clicked()), this, SLOT(clickedSnippetButton()));
	connect(m_maximizeButton, SIGNAL(clicked()), this, SLOT(clickedMaximizeButton()));

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
	const std::string& code,
	std::shared_ptr<TokenLocationFile> locationFile
){
	std::shared_ptr<QtCodeSnippet> snippet(
		new QtCodeSnippet(startLineNumber, title, code, locationFile, this));

	layout()->addWidget(snippet.get());

	if (locationFile->isWholeCopy)
	{
		m_fileSnippet = snippet;
		clickedMaximizeButton();
		return;
	}

	m_snippets.push_back(snippet);

	if (m_parent->getShowMaximizeButton())
	{
		snippet->addMaximizeButton();
	}

	int maxDigits = 1;
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		maxDigits = qMax(maxDigits, snippet->lineNumberDigits());
	}

	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		snippet->updateLineNumberAreaWidthForDigits(maxDigits);
	}

	clickedSnippetButton();
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

	m_title->setEnabled(m_parent->getShowMaximizeButton());
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
		MessageShowFile(m_filePath.absoluteStr(), 0, 0).dispatch();
	}

	m_minimizeButton->setEnabled(true);
	if (m_snippets.size())
	{
		m_snippetButton->setEnabled(true);
	}
	m_maximizeButton->setEnabled(false);
}
