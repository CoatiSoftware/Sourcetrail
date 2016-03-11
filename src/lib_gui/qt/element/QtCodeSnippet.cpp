#include "qt/element/QtCodeSnippet.h"

#include <QBoxLayout>
#include <qmenu.h>
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

	std::string code = first->getCode();

	std::string secondCode = second->getCode();
	int secondCodeStartIndex = 0;
	for (uint i = second->getStartLineNumber(); i <= first->getEndLineNumber(); i++)
	{
		secondCodeStartIndex = secondCode.find("\n", secondCodeStartIndex) + 1;
	}
	code += secondCode.substr(secondCodeStartIndex, secondCode.npos);

	CodeSnippetParams params;
	params.startLineNumber = first->getStartLineNumber();
	params.title = first->m_titleString;
	params.titleId = first->m_titleId;
	params.footer = second->m_footerString;
	params.footerId = second->m_footerId;
	params.code = code;
	params.locationFile = locationFile;

	return std::shared_ptr<QtCodeSnippet>(new QtCodeSnippet(params, file));
}

QtCodeSnippet::QtCodeSnippet(const CodeSnippetParams& params, QtCodeFile* file)
	: QFrame(file)
	, m_titleId(params.titleId)
	, m_titleString(params.title)
	, m_footerId(params.footerId)
	, m_footerString(params.footer)
	, m_title(nullptr)
	, m_footer(nullptr)
	, m_codeArea(std::make_shared<QtCodeArea>(params.startLineNumber, params.code, params.locationFile, file, this))
{
	setObjectName("code_snippet");

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	if (m_titleString.size())
	{
		m_title = createScopeLine(layout);
		m_title->setText(FilePath(m_titleString).fileName().c_str());
		connect(m_title, SIGNAL(clicked()), this, SLOT(clickedTitle()));
	}

	layout->addWidget(m_codeArea.get());

	if (m_footerString.size())
	{
		m_footer = createScopeLine(layout);
		m_footer->setText(FilePath(m_footerString).fileName().c_str());
		connect(m_footer, SIGNAL(clicked()), this, SLOT(clickedFooter()));
	}

	if (params.reduced)
	{
		m_codeArea->hideLineNumbers();
	}
}

QtCodeSnippet::~QtCodeSnippet()
{
}

QtCodeFile* QtCodeSnippet::getFile() const
{
	return m_codeArea->getFile();
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

void QtCodeSnippet::setIsActiveFile(bool isActiveFile)
{
	m_codeArea->setIsActiveFile(isActiveFile);
}

uint QtCodeSnippet::getFirstActiveLineNumber() const
{
	return m_codeArea->getFirstActiveLineNumber();
}

QRectF QtCodeSnippet::getLineRectForLineNumber(uint lineNumber) const
{
	return m_codeArea->getLineRectForLineNumber(lineNumber);
}

std::string QtCodeSnippet::getCode() const
{
	return m_codeArea->getCode();
}

void QtCodeSnippet::contextMenuEvent(QContextMenuEvent* event)
{
	// QMenu menu(this);
	// menu.addAction(new QAction("Bar", this));
	// menu.addAction(new QAction("Brew Tea", this));
	// menu.addAction(new QAction("Translate", this));
	// menu.exec(event->globalPos());
}

void QtCodeSnippet::clickedTitle()
{
	if (m_titleId > 0)
	{
		MessageShowScope(m_titleId, dynamic_cast<QtCodeFile*>(parent())->hasErrors()).dispatch();
	}
	else
	{
		dynamic_cast<QtCodeFile*>(parent())->clickedMaximizeButton();
	}
}

void QtCodeSnippet::clickedFooter()
{
	if (m_footerId > 0)
	{
		MessageShowScope(m_footerId, dynamic_cast<QtCodeFile*>(parent())->hasErrors()).dispatch();
	}
}

QPushButton* QtCodeSnippet::createScopeLine(QBoxLayout* layout)
{
	QHBoxLayout* lineLayout = new QHBoxLayout();
	lineLayout->setMargin(0);
	lineLayout->setSpacing(0);
	lineLayout->setAlignment(Qt::AlignLeft);
	layout->addLayout(lineLayout);

	QPushButton* dots = new QPushButton(this);
	dots->setObjectName("dots");
	dots->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	lineLayout->addWidget(dots);
	m_dots.push_back(dots);

	QPushButton* line = new QPushButton(this);
	line->setObjectName("scope_name");
	line->minimumSizeHint(); // force font loading
	line->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	lineLayout->addWidget(line);

	return line;
}

void QtCodeSnippet::updateDots()
{
	for (QPushButton* dots : m_dots)
	{
		dots->setText(QString::fromStdString(std::string(lineNumberDigits(), '.')));
		dots->setMinimumWidth(m_codeArea->lineNumberAreaWidth());
	}
}
