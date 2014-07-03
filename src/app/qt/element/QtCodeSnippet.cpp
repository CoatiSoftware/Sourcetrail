#include "qt/element/QtCodeSnippet.h"

#include <QtWidgets>

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"
#include "qt/view/QtCodeView.h"

QtCodeSnippet::LineNumberArea::LineNumberArea(QtCodeSnippet *codeSnippet)
	: QWidget(codeSnippet)
	, m_codeSnippet(codeSnippet)
{
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


QtCodeSnippet::QtCodeSnippet(QtCodeView* parentView, int startLineNumber, QWidget *parent)
	: QPlainTextEdit(parent)
	, m_parentView(parentView)
	, m_startLineNumber(startLineNumber)
{
	m_lineNumberArea = new LineNumberArea(this);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(clickTokenLocation()));

	updateLineNumberAreaWidth(0);
}

QtCodeSnippet::~QtCodeSnippet()
{
}

void QtCodeSnippet::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(m_lineNumberArea);
	painter.fillRect(event->rect(), QColor(225,225,225));

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
			painter.drawText(0, top, m_lineNumberArea->width() - 1, fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + static_cast<int>(blockBoundingRect(block).height());
		blockNumber++;
	}
}

int QtCodeSnippet::lineNumberAreaWidth()
{
	int digits = 1;
	int max = qMax(1, m_startLineNumber + blockCount());

	while (max >= 10)
	{
		max /= 10;
		digits++;
	}

	int width = 2 + fontMetrics().width(QLatin1Char('9')) * digits;
	return width;
}

void QtCodeSnippet::annotateText(const TokenLocationFile& locationFile)
{
	QList<QTextEdit::ExtraSelection> extraSelections;

	for (const TokenLocationFile::TokenLocationLinePairType& lineItem : locationFile.getTokenLocationLines())
	{
		for (const TokenLocationLine::TokenLocationPairType& locationItem : lineItem.second->getTokenLocations())
		{
			TokenLocation* location = locationItem.second.get();
			if (!location->isStartTokenLocation())
			{
				continue;
			}

			Annotation annotation;
			annotation.start = toTextEditPosition(location->getLineNumber(), location->getColumnNumber() - 1);
			annotation.end = toTextEditPosition(
				location->getEndTokenLocation()->getLineNumber(),
				location->getEndTokenLocation()->getColumnNumber());
			annotation.tokenId = location->getTokenId();
			m_annotations.push_back(annotation);

			QTextEdit::ExtraSelection selection;

			QColor color = QColor(Qt::red);
			color.setAlphaF(0.3f);
			selection.format.setBackground(color);

			selection.cursor = textCursor();
			selection.cursor.clearSelection();
			selection.cursor.setPosition(annotation.start);
			selection.cursor.setPosition(annotation.end, QTextCursor::KeepAnchor);

			extraSelections.append(selection);
		}
	}

	setExtraSelections(extraSelections);
}

void QtCodeSnippet::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
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
	for (Annotation annotation : m_annotations)
	{
		if (clickPosition >= annotation.start && clickPosition <= annotation.end)
		{
			m_parentView->activateToken(annotation.tokenId);
			return;
		}
	}
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
