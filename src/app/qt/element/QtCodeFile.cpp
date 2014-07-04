#include "qt/element/QtCodeFile.h"

#include <QLabel>
#include <QVBoxLayout>

#include "qt/element/QtCodeSnippet.h"

QtCodeFile::QtCodeFile(QtCodeView* parentView, const std::string& fileName, QWidget *parent)
	: QWidget(parent)
	, m_parentView(parentView)
	, m_fileName(fileName)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(2);
	setLayout(layout);

	QLabel* label = new QLabel(fileName.c_str(), this);
	QFontMetrics metrics(label->font());

	label->setStyleSheet("background-color: #E1E1E1; padding: 3px;");
	label->setFixedWidth(metrics.boundingRect(fileName.c_str()).width() + 12);
	layout->addWidget(label);
}

QtCodeFile::~QtCodeFile()
{
}

const std::string& QtCodeFile::getFileName() const
{
	return m_fileName;
}

void QtCodeFile::addCodeSnippet(const std::string& str, const TokenLocationFile& locationFile, int startLineNumber)
{
	std::shared_ptr<QtCodeSnippet> snippet =
		std::make_shared<QtCodeSnippet>(m_parentView, str, locationFile, startLineNumber, this);
	layout()->addWidget(snippet.get());
	m_snippets.push_back(snippet);
}
