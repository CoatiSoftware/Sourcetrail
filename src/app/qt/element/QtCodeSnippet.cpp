#include "qt/element/QtCodeSnippet.h"

#include <QFont>
#include <QtWidgets>

#include "ApplicationSettings.h"
#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "qt/utility/QtHighLighter.h"
#include "qt/view/QtCodeView.h"

QtCodeSnippet::LineNumberArea::LineNumberArea(QtCodeSnippet *codeSnippet)
	: QWidget(codeSnippet)
	, m_codeSnippet(codeSnippet)
{
	setObjectName("line_number_area");
}

QtCodeSnippet::LineNumberArea::~LineNumberArea()
{
}

QSize QtCodeSnippet::LineNumberArea::sizeHint() const
{
	return QSize(m_codeSnippet->lineNumberAreaWidth(), 0);
}

void QtCodeSnippet::LineNumberArea::paintEvent(QPaintEvent *event)
{
	m_codeSnippet->lineNumberAreaPaintEvent(event);
}


QtCodeSnippet::QtCodeSnippet(
	QtCodeView* parentView,
	int startLineNumber,
	const std::string& code,
	const TokenLocationFile& locationFile,
	const std::vector<Id>& activeTokenIds,
	QWidget *parent
)
	: QPlainTextEdit(parent)
	, m_parentView(parentView)
	, m_startLineNumber(startLineNumber)
	, m_activeTokenIds(activeTokenIds)
	, m_digits(0)
{
	setObjectName("code_snippet");
	m_lineNumberArea = new LineNumberArea(this);

	setReadOnly(true);
	setFrameStyle(QFrame::NoFrame);
	setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setLineWrapMode(QPlainTextEdit::NoWrap);

	m_highlighter = new QtHighlighter(document());

	std::string displayCode = code;
	if (*code.rbegin() == '\n')
	{
		displayCode.pop_back();
	}

	setPlainText(QString::fromUtf8(displayCode.c_str()));
	annotateText(locationFile);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(clickTokenLocation()));
	connect(this, SIGNAL(selectionChanged()), this, SLOT(clearSelection()));

	m_digits = lineNumberDigits();
	updateLineNumberAreaWidth(0);
}

QtCodeSnippet::~QtCodeSnippet()
{
}

QSize QtCodeSnippet::sizeHint() const
{
	int width = lineNumberAreaWidth();
	int height = (document()->size().height() + 0.7f) * fontMetrics().lineSpacing();
	return QSize(width, height);
}

void QtCodeSnippet::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(m_lineNumberArea);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom = top + static_cast<int>(blockBoundingRect(block).height());

	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			QString number = QString::number(blockNumber + m_startLineNumber);
			painter.setPen(Qt::black);
			painter.drawText(0, top, m_lineNumberArea->width() - 13, fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + static_cast<int>(blockBoundingRect(block).height());
		blockNumber++;
	}
}

int QtCodeSnippet::lineNumberDigits() const
{
	int digits = 1;
	int max = qMax(1, m_startLineNumber + blockCount());

	while (max >= 10)
	{
		max /= 10;
		digits++;
	}
	return digits;
}

int QtCodeSnippet::lineNumberAreaWidth() const
{
	return fontMetrics().width(QLatin1Char('9')) * m_digits + 30;
}

void QtCodeSnippet::updateLineNumberAreaWidthForDigits(int digits)
{
	m_digits = digits;
	updateLineNumberAreaWidth(0);
}

void QtCodeSnippet::annotateText(const TokenLocationFile& locationFile)
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	locationFile.forEachTokenLocation(
		[&](TokenLocation* location)
		{
			if (location->isEndTokenLocation() && location->getStartTokenLocation())
			{
				return;
			}

			Annotation annotation;
			if (location->isStartTokenLocation())
			{
				annotation.start = toTextEditPosition(location->getLineNumber(), location->getColumnNumber() - 1);
			}
			else
			{
				annotation.start = startTextEditPosition();
			}

			TokenLocation* endLocation = location->getEndTokenLocation();
			if (endLocation)
			{
				annotation.end = toTextEditPosition(endLocation->getLineNumber(), endLocation->getColumnNumber());
			}
			else
			{
				annotation.end = endTextEditPosition();
			}

			annotation.tokenId = location->getTokenId();
			m_annotations.push_back(annotation);

			Colori color;
			const std::vector<Id>& ids = m_activeTokenIds;
			bool isActive = std::find(ids.begin(), ids.end(), location->getTokenId()) != ids.end();
			if (isActive)
			{
				color = ApplicationSettings::getInstance()->getCodeActiveLinkColor();
			}
			else
			{
				color = ApplicationSettings::getInstance()->getCodeLinkColor();
			}

			QTextEdit::ExtraSelection selection;
			selection.format.setBackground(QColor(color.r, color.g, color.b, color.a));

			selection.cursor = textCursor();
			selection.cursor.clearSelection();
			selection.cursor.setPosition(annotation.start);
			selection.cursor.setPosition(annotation.end, QTextCursor::KeepAnchor);

			extraSelections.append(selection);
		}
	);

	setExtraSelections(extraSelections);
}

void QtCodeSnippet::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void QtCodeSnippet::showEvent(QShowEvent* event)
{
	int tabWidth = ApplicationSettings::getInstance()->getCodeTabWidth();
	setTabStopWidth(tabWidth * fontMetrics().width('9'));

	setMaximumHeight(sizeHint().height());
}

void QtCodeSnippet::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void QtCodeSnippet::updateLineNumberArea(const QRect &rect, int dy)
{
	if (dy)
	{
		m_lineNumberArea->scroll(0, dy);
	}
	else
	{
		m_lineNumberArea->update(0, rect.y(), m_lineNumberArea->width(), rect.height());
	}

	if (rect.contains(viewport()->rect()))
	{
		updateLineNumberAreaWidth(0);
	}
}

void QtCodeSnippet::clickTokenLocation()
{
	int clickPosition = textCursor().position();
	int diff = endTextEditPosition() + 1;
	Id tokenId = 0;

	for (Annotation annotation : m_annotations)
	{
		if (clickPosition >= annotation.start && clickPosition <= annotation.end)
		{
			int d = annotation.end - annotation.start;
			if (d < diff)
			{
				diff = d;
				tokenId = annotation.tokenId;
			}
		}
	}

	if (tokenId)
	{
		m_parentView->activateToken(tokenId);
	}
}

void QtCodeSnippet::clearSelection()
{
	QTextCursor cursor = textCursor();
	cursor.clearSelection();
	setTextCursor(cursor);
}

int QtCodeSnippet::toTextEditPosition(int lineNumber, int columnNumber) const
{
	lineNumber -= m_startLineNumber - 1;
	int position = 0;

	for (int i = 0; i < lineNumber - 1; i++)
	{
		position += document()->findBlockByLineNumber(i).length();
	}

	position += columnNumber;
	return position;
}

int QtCodeSnippet::startTextEditPosition() const
{
	return 0;
}

int QtCodeSnippet::endTextEditPosition() const
{
	int position = 0;

	for (int i = 0; i < document()->blockCount(); i++)
	{
		position += document()->findBlockByLineNumber(i).length();
	}

	return position - 1;
}
