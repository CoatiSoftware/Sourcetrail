#include "qt/element/QtCodeFile.h"

#include <QLabel>
#include <QVBoxLayout>

#include "qt/element/QtCodeFileList.h"
#include "qt/element/QtCodeSnippet.h"
#include "utility/FileSystem.h"

QtCodeFile::QtCodeFile(const std::string& filePath, QtCodeFileList* parent)
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

const std::string& QtCodeFile::getFilePath() const
{
	return m_filePath;
}

std::string QtCodeFile::getFileName() const
{
	return FileSystem::fileName(m_filePath);
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
}
