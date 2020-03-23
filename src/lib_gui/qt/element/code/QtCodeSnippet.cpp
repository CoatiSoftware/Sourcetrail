#include "QtCodeSnippet.h"

#include <QBoxLayout>
#include <QPushButton>
#include <QStyle>
#include <qmenu.h>

#include "MessageShowScope.h"

#include "QtCodeFile.h"
#include "QtCodeNavigator.h"
#include "QtHoverButton.h"
#include "SourceLocationFile.h"

QtCodeSnippet::QtCodeSnippet(const CodeSnippetParams& params, QtCodeNavigator* navigator, QtCodeFile* file)
	: QFrame(file)
	, m_navigator(navigator)
	, m_file(file)
	, m_titleId(params.titleId)
	, m_titleString(params.title)
	, m_footerId(params.footerId)
	, m_footerString(params.footer)
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
		m_titleDots = m_dots.back();
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
		m_footerDots = m_dots.back();
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

	updateScopeLineFocus(m_title, m_titleDots);
	updateScopeLineFocus(m_footer, m_footerDots);
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

bool QtCodeSnippet::hasFocus(const CodeFocusHandler::Focus& focus) const
{
	return m_codeArea == focus.area;
}

bool QtCodeSnippet::setFocus(Id locationId)
{
	return m_codeArea->setFocus(locationId);
}

bool QtCodeSnippet::moveFocus(const CodeFocusHandler::Focus& focus, CodeFocusHandler::Direction direction)
{
	if (m_codeArea == focus.area)
	{
		if (focus.scopeLine)
		{
			if (m_title == focus.scopeLine && direction == CodeFocusHandler::Direction::DOWN)
			{
				return m_codeArea->moveFocus(direction, m_codeArea->getStartLineNumber() - 1, 0);
			}
			else if (m_footer == focus.scopeLine && direction == CodeFocusHandler::Direction::UP)
			{
				return m_codeArea->moveFocus(direction, m_codeArea->getEndLineNumber() + 1, 0);
			}
		}
		else
		{
			bool moved = m_codeArea->moveFocus(direction, focus.lineNumber, focus.locationId);
			if (!moved)
			{
				if (m_title && direction == CodeFocusHandler::Direction::UP)
				{
					m_navigator->setFocusedScopeLine(m_codeArea, m_title, getStartLineNumber());
					return true;
				}
				else if (m_footer && direction == CodeFocusHandler::Direction::DOWN)
				{
					m_navigator->setFocusedScopeLine(m_codeArea, m_footer, getEndLineNumber());
					return true;
				}
			}
			return moved;
		}
	}
	return false;
}

void QtCodeSnippet::focusTop()
{
	if (m_title)
	{
		m_navigator->setFocusedScopeLine(m_codeArea, m_title, getStartLineNumber());
	}
	else
	{
		m_codeArea->moveFocus(
			CodeFocusHandler::Direction::DOWN, m_codeArea->getStartLineNumber() - 1, 0);
	}
}

void QtCodeSnippet::focusBottom()
{
	if (m_footer)
	{
		m_navigator->setFocusedScopeLine(m_codeArea, m_footer, getEndLineNumber());
	}
	else
	{
		m_codeArea->moveFocus(CodeFocusHandler::Direction::UP, m_codeArea->getEndLineNumber() + 1, 0);
	}
}

void QtCodeSnippet::copySelection()
{
	m_codeArea->copySelection();
}

void QtCodeSnippet::ensureLocationIdVisible(Id locationId, bool animated)
{
	m_codeArea->ensureLocationIdVisible(locationId, width(), animated);
}

void QtCodeSnippet::clickedTitle()
{
	m_codeArea->moveFocusToLine(static_cast<int>(m_codeArea->getStartLineNumber()), 0, false);

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
	m_codeArea->moveFocusToLine(static_cast<int>(m_codeArea->getEndLineNumber()), 0, true);

	if (m_footerId > 0)
	{
		MessageShowScope(m_footerId, m_navigator->hasErrors()).dispatch();
	}
	else
	{
		getFile()->requestWholeFileContent(getEndLineNumber());
	}
}

QtHoverButton* QtCodeSnippet::createScopeLine(QBoxLayout* layout)
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

	QtHoverButton* line = new QtHoverButton(this);
	line->setObjectName(QStringLiteral("scope_name"));
	line->minimumSizeHint();							// force font loading
	line->setAttribute(Qt::WA_LayoutUsesWidgetRect);	// fixes layouting on Mac
	line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	lineLayout->addWidget(line);

	connect(line, &QtHoverButton::hoveredIn, [this, line]() {
		m_navigator->setFocusedScopeLine(m_codeArea, line, 0);
		m_navigator->setFocus();
	});

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

void QtCodeSnippet::updateScopeLineFocus(QPushButton* line, QPushButton* dots)
{
	if (line && dots)
	{
		bool focus = line == m_navigator->getCurrentFocus().scopeLine;
		if (focus != dots->property("focused").toBool())
		{
			line->setProperty("focused", focus);
			line->style()->polish(line);	// recomputes style to make property take effect

			dots->setProperty("focused", focus);
			dots->style()->polish(dots);	// recomputes style to make property take effect
		}
	}
}
