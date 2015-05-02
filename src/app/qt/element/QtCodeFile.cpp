#include "qt/element/QtCodeFile.h"

#include <QPushButton>
#include <QVBoxLayout>

#include "utility/messaging/type/MessageShowFile.h"

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

	m_title = new QPushButton(filePath.fileName().c_str(), this);
	m_title->setObjectName("title_label");
	m_title->minimumSizeHint(); // force font loading
	m_title->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	m_title->setToolTip(QString::fromStdString(filePath.str()));
	m_title->setFixedWidth(m_title->fontMetrics().width(filePath.fileName().c_str()) + 32);
	m_title->setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	layout->addWidget(m_title);

	connect(m_title, SIGNAL(clicked()), this, SLOT(clickedTitle()));

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
	const std::string& code,
	const TokenLocationFile& locationFile
){
	std::shared_ptr<QtCodeSnippet> snippet(
		new QtCodeSnippet(startLineNumber, code, locationFile, this));

	if (m_parent->getShowMaximizeButton())
	{
		snippet->addMaximizeButton();
	}

	layout()->addWidget(snippet.get());
	m_snippets.push_back(snippet);

	int maxDigits = 1;
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		maxDigits = qMax(maxDigits, snippet->lineNumberDigits());
	}

	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		snippet->updateLineNumberAreaWidthForDigits(maxDigits);
	}
}

void QtCodeFile::update()
{
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		snippet->update();
	}

	m_title->setEnabled(m_parent->getShowMaximizeButton());
}

void QtCodeFile::clickedTitle()
{
	MessageShowFile(m_filePath.absoluteStr(), 0, 0).dispatch();
}
