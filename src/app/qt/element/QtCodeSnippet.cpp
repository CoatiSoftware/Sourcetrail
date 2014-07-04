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
	const std::string& code,
	const TokenLocationFile& locationFile,
	int startLineNumber,
	Id activeTokenId,
	QWidget *parent
)
	: QPlainTextEdit(parent)
	, m_parentView(parentView)
	, m_startLineNumber(startLineNumber)
	, m_activeTokenId(activeTokenId)
{
	m_lineNumberArea = new LineNumberArea(this);

	setReadOnly(true);
	setFrameStyle(QFrame::NoFrame);
	setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setLineWrapMode(QPlainTextEdit::NoWrap);

	QFont font;
	font.setFamily(ApplicationSettings::getInstance()->getCodeFontName().c_str());
	font.setPointSize(ApplicationSettings::getInstance()->getCodeFontSize());
	font.setFixedPitch(true);
	setFont(font);

	int tabWidth = ApplicationSettings::getInstance()->getCodeTabWidth();
	QFontMetrics metrics(font);
	setTabStopWidth(tabWidth * metrics.width(' '));

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

	setMaximumHeight(sizeHint().height());

	updateLineNumberAreaWidth(0);
}

QtCodeSnippet::~QtCodeSnippet()
{
}

QSize QtCodeSnippet::sizeHint() const
{
	int width = lineNumberAreaWidth() + document()->size().width();
	int height = (document()->size().height() + 1) * QFontMetrics(font()).lineSpacing();
	return QSize(width, height);
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
			painter.drawText(0, top, m_lineNumberArea->width() - 3, fontMetrics().height(), Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + static_cast<int>(blockBoundingRect(block).height());
		blockNumber++;
	}
}

int QtCodeSnippet::lineNumberAreaWidth() const
{
	int digits = 1;
	int max = qMax(1, m_startLineNumber + blockCount());

	while (max >= 10)
	{
		max /= 10;
		digits++;
	}

	int width = fontMetrics().width(QLatin1Char('9')) * digits + 6;
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

			Colori color;
			if (location->getTokenId() == m_activeTokenId)
			{
				color = ApplicationSettings::getInstance()->getCodeActiveLinkColor();
			}
			else
			{
				color = ApplicationSettings::getInstance()->getCodeLinkColor();
			}

			selection.format.setBackground(QColor(color.r, color.g, color.b, color.a));

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
