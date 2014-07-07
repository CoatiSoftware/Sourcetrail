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
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	QLabel* label = new QLabel(fileName.c_str(), this);
	QFontMetrics metrics(label->font());

	label->setStyleSheet("background-color: #E1E1E1; padding: 3px;");
	label->setFixedWidth(metrics.boundingRect(fileName.c_str()).width() + 12);
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
	const std::string& str, const TokenLocationFile& locationFile, int startLineNumber, Id activeTokenId
){
	std::shared_ptr<QtCodeSnippet> snippet(
		new QtCodeSnippet(m_parentView, str, locationFile, startLineNumber, activeTokenId, this)
	);
	layout()->addWidget(snippet.get());
	m_snippets.push_back(snippet);
}
