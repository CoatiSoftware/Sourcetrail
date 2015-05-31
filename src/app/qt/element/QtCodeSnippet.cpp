#include "qt/element/QtCodeSnippet.h"

#include <QBoxLayout>
#include <QPushButton>

#include "qt/element/QtCodeFile.h"

QtCodeSnippet::QtCodeSnippet(
	uint startLineNumber,
	const std::string& title,
	const std::string& code,
	std::shared_ptr<TokenLocationFile> locationFile,
	QtCodeFile* file
)
	: QFrame(file)
	, m_dots(nullptr)
	, m_title(nullptr)
	, m_codeArea(std::make_shared<QtCodeArea>(startLineNumber, code, locationFile, file, this))
{
	setObjectName("code_snippet");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	if (title.size())
	{
		QHBoxLayout* titleLayout = new QHBoxLayout();
		titleLayout->setMargin(0);
		titleLayout->setSpacing(0);
		titleLayout->setAlignment(Qt::AlignLeft);
		layout->addLayout(titleLayout);

		m_dots = new QPushButton(this);
		m_dots->setObjectName("dots");
		m_dots->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
		titleLayout->addWidget(m_dots);

		m_title = new QPushButton(title.c_str(), this);
		m_title->setObjectName("scope_name");
		m_title->minimumSizeHint(); // force font loading
		m_title->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
		m_title->setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
		titleLayout->addWidget(m_title);
	}

	layout->addWidget(m_codeArea.get());
	updateDots();
}

QtCodeSnippet::~QtCodeSnippet()
{
}

int QtCodeSnippet::lineNumberDigits() const
{
	return m_codeArea->lineNumberDigits();
}

void QtCodeSnippet::updateLineNumberAreaWidthForDigits(int digits)
{
	m_codeArea->updateLineNumberAreaWidthForDigits(digits);
	updateDots();
}

void QtCodeSnippet::updateContent()
{
	m_codeArea->updateContent();
	updateDots();
}

bool QtCodeSnippet::isActive() const
{
	return m_codeArea->isActive();
}

void QtCodeSnippet::updateDots()
{
	if (!m_dots)
	{
		return;
	}

	m_dots->setText(QString::fromStdString(std::string(lineNumberDigits(), '.')));
	m_dots->setMinimumWidth(m_codeArea->lineNumberAreaWidth());
}
