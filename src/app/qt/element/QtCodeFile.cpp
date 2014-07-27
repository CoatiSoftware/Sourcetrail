#include "qt/element/QtCodeFile.h"

#include <QLabel>
#include <QVBoxLayout>

#include "qt/element/QtCodeSnippet.h"

QtCodeFile::QtCodeFile(const std::string& fileName, QWidget *parent)
	: QWidget(parent)
	, m_fileName(fileName)
{
	setObjectName("code_file");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	QLabel* label = new QLabel(fileName.c_str(), this);
	label->setObjectName("title_label");
	label->minimumSizeHint(); // force font loading
	label->setFixedWidth(label->fontMetrics().width(fileName.c_str()) + 32);
	label->setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	layout->addWidget(label);
}

QtCodeFile::~QtCodeFile()
{
}

const std::string& QtCodeFile::getFileName() const
{
	return m_fileName;
}

void QtCodeFile::addCodeSnippet(
	int startLineNumber,
	const std::string& code,
	const TokenLocationFile& locationFile,
	const std::vector<Id>& activeTokenIds
){
	std::shared_ptr<QtCodeSnippet> snippet(
		new QtCodeSnippet(startLineNumber, code, locationFile, activeTokenIds, this));
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
