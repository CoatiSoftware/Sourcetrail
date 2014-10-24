#include "qt/element/QtCodeFile.h"

#include <QLabel>
#include <QVBoxLayout>

#include "qt/element/QtCodeSnippet.h"
#include "utility/FileSystem.h"

QtCodeFile::QtCodeFile(const std::string& filePath, QWidget *parent)
	: QWidget(parent)
	, m_filePath(filePath)
	, m_showMaximizeButton(true)
{
	setObjectName("code_file");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	QLabel* label = new QLabel(FileSystem::fileName(filePath).c_str(), this);
	label->setObjectName("title_label");
	label->minimumSizeHint(); // force font loading
	label->setFixedWidth(label->fontMetrics().width(FileSystem::fileName(filePath).c_str()) + 32);
	label->setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	layout->addWidget(label);
}

QtCodeFile::~QtCodeFile()
{
}

std::string QtCodeFile::getFileName() const
{
	return FileSystem::fileName(m_filePath);
}

void QtCodeFile::addCodeSnippet(
	uint startLineNumber,
	const std::string& code,
	const TokenLocationFile& locationFile,
	const std::vector<Id>& activeTokenIds
){
	std::shared_ptr<QtCodeSnippet> snippet(
		new QtCodeSnippet(startLineNumber, code, locationFile, activeTokenIds, this));

	if (m_showMaximizeButton)
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

void QtCodeFile::setActiveTokenIds(const std::vector<Id>& activeTokenIds)
{
	for (std::shared_ptr<QtCodeSnippet> snippet : m_snippets)
	{
		snippet->setActiveTokenIds(activeTokenIds);
	}
}

void QtCodeFile::setShowMaximizeButton(bool show)
{
	m_showMaximizeButton = show;
}
