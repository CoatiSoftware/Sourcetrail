#include "QtCodeSnippet.h"

#include <QBoxLayout>
#include <QPushButton>
#include <qmenu.h>

#include "MessageShowScope.h"

#include "QtCodeFile.h"
#include "QtCodeNavigator.h"
#include "SourceLocationFile.h"

QtCodeSnippet::QtCodeSnippet(const CodeSnippetParams& params, QtCodeNavigator* navigator, QtCodeFile* file)
	: QFrame(file)
	, m_navigator(navigator)
	, m_file(file)
	, m_titleId(params.titleId)
	, m_titleString(params.title)
	, m_footerId(params.footerId)
	, m_footerString(params.footer)
	, m_title(nullptr)
	, m_footer(nullptr)
	, m_codeArea(nullptr)
{
	setObjectName(QStringLiteral("code_snippet"));

	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	if (!m_titleString.empty() && !params.isOverview)
	{
		m_title = createScopeLine(layout);
		if (m_titleId == 0)	   // title is a file path
		{
			m_title->setText(QString::fromStdWString(FilePath(m_titleString).fileName()));
		}
		else
		{
			m_title->setText(QString::fromStdWString(m_titleString));
		}
		connect(m_title, &QPushButton::clicked, this, &QtCodeSnippet::clickedTitle);
	}

	m_codeArea = new QtCodeArea(
		params.startLineNumber, params.code, params.locationFile, navigator, !params.isOverview, this);
	layout->addWidget(m_codeArea);

	if (!m_footerString.empty())
	{
		m_footer = createScopeLine(layout);
		if (m_footerId == 0)	// footer is a file path
		{
			m_footer->setText(QString::fromStdWString(FilePath(m_footerString).fileName()));
		}
		else
		{
			m_footer->setText(QString::fromStdWString(m_footerString));
		}
		connect(m_footer, &QPushButton::clicked, this, &QtCodeSnippet::clickedFooter);
	}
}

QtCodeSnippet::~QtCodeSnippet() {}

QtCodeFile* QtCodeSnippet::getFile() const
{
	return m_file;
}

QtCodeArea* QtCodeSnippet::getArea() const
{
	return m_codeArea;
}

size_t QtCodeSnippet::getStartLineNumber() const
{
	return m_codeArea->getStartLineNumber();
}

size_t QtCodeSnippet::getEndLineNumber() const
{
	return m_codeArea->getEndLineNumber();
}

int QtCodeSnippet::lineNumberDigits() const
{
	return m_codeArea->lineNumberDigits();
}

void QtCodeSnippet::updateSourceLocations(const CodeSnippetParams& params)
{
	m_codeArea->updateSourceLocations(params.locationFile);

	ensureLocationIdVisible(m_codeArea->getLocationIdOfFirstHighlightedLocation(), false);
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

void QtCodeSnippet::setIsActiveFile(bool isActiveFile)
{
	m_codeArea->setIsActiveFile(isActiveFile);
}

size_t QtCodeSnippet::getLineNumberForLocationId(Id locationId) const
{
	return m_codeArea->getLineNumberForLocationId(locationId);
}

std::pair<size_t, size_t> QtCodeSnippet::getLineNumbersForLocationId(Id locationId) const
{
	return m_codeArea->getLineNumbersForLocationId(locationId);
}

Id QtCodeSnippet::getFirstActiveLocationId(Id tokenId) const
{
	Id scopeId = m_codeArea->getLocationIdOfFirstActiveScopeLocation(tokenId);
	if (scopeId)
	{
		return scopeId;
	}

	return m_codeArea->getLocationIdOfFirstActiveLocation(tokenId);
}

QRectF QtCodeSnippet::getLineRectForLineNumber(size_t lineNumber) const
{
	return m_codeArea->getLineRectForLineNumber(lineNumber);
}

std::string QtCodeSnippet::getCode() const
{
	return m_codeArea->getCode();
}

void QtCodeSnippet::findScreenMatches(
	const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches)
{
	m_codeArea->findScreenMatches(query, screenMatches);
}

void QtCodeSnippet::ensureLocationIdVisible(Id locationId, bool animated)
{
	m_codeArea->ensureLocationIdVisible(locationId, width(), animated);
}

void QtCodeSnippet::clickedTitle()
{
	if (m_titleId > 0)
	{
		MessageShowScope(m_titleId, m_navigator->hasErrors()).dispatch();
	}
	else
	{
		getFile()->requestWholeFileContent(getStartLineNumber());
	}
}

void QtCodeSnippet::clickedFooter()
{
	if (m_footerId > 0)
	{
		MessageShowScope(m_footerId, m_navigator->hasErrors()).dispatch();
	}
	else
	{
		getFile()->requestWholeFileContent(getEndLineNumber());
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
	dots->setObjectName(QStringLiteral("dots"));
	dots->setAttribute(Qt::WA_LayoutUsesWidgetRect);	// fixes layouting on Mac
	lineLayout->addWidget(dots);
	m_dots.push_back(dots);

	QPushButton* line = new QPushButton(this);
	line->setObjectName(QStringLiteral("scope_name"));
	line->minimumSizeHint();							// force font loading
	line->setAttribute(Qt::WA_LayoutUsesWidgetRect);	// fixes layouting on Mac
	line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	lineLayout->addWidget(line);

	return line;
}

void QtCodeSnippet::updateDots()
{
	for (QPushButton* dots: m_dots)
	{
		dots->setText(QString::fromStdString(std::string(lineNumberDigits(), '.')));
		dots->setMinimumWidth(m_codeArea->lineNumberAreaWidth());
	}
}
