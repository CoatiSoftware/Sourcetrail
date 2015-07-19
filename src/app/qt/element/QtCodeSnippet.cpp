#include "qt/element/QtCodeSnippet.h"

#include <QBoxLayout>
#include <QPushButton>

#include "utility/messaging/type/MessageShowScope.h"
#include "utility/text/TextAccess.h"

#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"

std::shared_ptr<QtCodeSnippet> QtCodeSnippet::merged(QtCodeSnippet* a, QtCodeSnippet* b, QtCodeFile* file)
{
	QtCodeSnippet* first = a->getStartLineNumber() < b->getStartLineNumber() ? a : b;
	QtCodeSnippet* second = a->getStartLineNumber() > b->getStartLineNumber() ? a : b;

	TokenLocationFile* aFile = a->m_codeArea->getTokenLocationFile().get();
	TokenLocationFile* bFile = b->m_codeArea->getTokenLocationFile().get();

	std::shared_ptr<TokenLocationFile> locationFile = std::make_shared<TokenLocationFile>(aFile->getFilePath());

	aFile->forEachTokenLocation(
		[&locationFile](TokenLocation* loc)
		{
			locationFile->addTokenLocationAsPlainCopy(loc);
		}
	);

	bFile->forEachTokenLocation(
		[&locationFile](TokenLocation* loc)
		{
			locationFile->addTokenLocationAsPlainCopy(loc);
		}
	);

	std::string code;
	std::shared_ptr<TextAccess> textAccess = TextAccess::createFromFile(locationFile->getFilePath().str());
	for (const std::string& line: textAccess->getLines(first->getStartLineNumber(), second->getEndLineNumber()))
	{
		code += line;
	}

	std::string title = first->m_title ? first->m_title->text().toStdString() : "";

	return std::make_shared<QtCodeSnippet>(
		first->getStartLineNumber(),
		title,
		first->m_titleId,
		code,
		locationFile,
		file
	);
}

QtCodeSnippet::QtCodeSnippet(
	uint startLineNumber,
	const std::string& title,
	Id titleId,
	const std::string& code,
	std::shared_ptr<TokenLocationFile> locationFile,
	QtCodeFile* file
)
	: QFrame(file)
	, m_titleId(titleId)
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
		m_title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
		titleLayout->addWidget(m_title);

		connect(m_title, SIGNAL(clicked()), this, SLOT(clickedTitle()));
	}

	layout->addWidget(m_codeArea.get());
	updateDots();
}

QtCodeSnippet::~QtCodeSnippet()
{
}

uint QtCodeSnippet::getStartLineNumber() const
{
	return m_codeArea->getStartLineNumber();
}

uint QtCodeSnippet::getEndLineNumber() const
{
	return m_codeArea->getEndLineNumber();
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

void QtCodeSnippet::clickedTitle()
{
	if (m_titleId)
	{
		MessageShowScope(m_titleId).dispatch();
	}
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
