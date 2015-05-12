#include "qt/element/QtCodeArea.h"

#include <QFont>
#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <QToolTip>

#include "utility/messaging/type/MessageActivateTokenLocation.h"
#include "utility/messaging/type/MessageShowFile.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "qt/utility/QtHighlighter.h"
#include "settings/ApplicationSettings.h"

QtCodeArea::LineNumberArea::LineNumberArea(QtCodeArea *codeArea)
	: QWidget(codeArea)
	, m_codeArea(codeArea)
{
	setObjectName("line_number_area");
}

QtCodeArea::LineNumberArea::~LineNumberArea()
{
}

QSize QtCodeArea::LineNumberArea::sizeHint() const
{
	return QSize(m_codeArea->lineNumberAreaWidth(), 0);
}

void QtCodeArea::LineNumberArea::paintEvent(QPaintEvent *event)
{
	m_codeArea->lineNumberAreaPaintEvent(event);
}


QtCodeArea::QtCodeArea(
	uint startLineNumber,
	const std::string& code,
	std::shared_ptr<TokenLocationFile> locationFile,
	QtCodeFile* parent
)
	: QPlainTextEdit(parent)
	, m_parent(parent)
	, m_maximizeButton(nullptr)
	, m_startLineNumber(startLineNumber)
	, m_focusedTokenId(0)
	, m_isFocused(false)
	, m_digits(0)
{
	setObjectName("code_area");
	setReadOnly(true);
	setFrameStyle(QFrame::NoFrame);
	setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setLineWrapMode(QPlainTextEdit::NoWrap);

	m_lineNumberArea = new LineNumberArea(this);
	m_highlighter = new QtHighlighter(document());

	std::string displayCode = code;
	if (*code.rbegin() == '\n')
	{
		displayCode.pop_back();
	}

	setPlainText(QString::fromUtf8(displayCode.c_str()));
	createAnnotations(locationFile);
	annotateText();

	m_digits = lineNumberDigits();
	updateLineNumberAreaWidth(0);

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(selectionChanged()), this, SLOT(clearSelection()));

	this->setMouseTracking(true);
}

QtCodeArea::~QtCodeArea()
{
}

QSize QtCodeArea::sizeHint() const
{
	int width = 480;
	int height = (document()->size().height() + 0.7f) * fontMetrics().lineSpacing();
	return QSize(width, height);
}

void QtCodeArea::addMaximizeButton()
{
	QHBoxLayout* layout = new QHBoxLayout();
	layout->setMargin(0);
	layout->setSpacing(0);
	layout->setAlignment(Qt::AlignTop);
	setLayout(layout);

	m_maximizeButton = new QPushButton(this);
	m_maximizeButton->setObjectName("maximize_button");
	m_maximizeButton->setEnabled(false);
	layout->addWidget(m_maximizeButton);
	layout->setAlignment(m_maximizeButton, Qt::AlignRight);

	connect(m_maximizeButton, SIGNAL(clicked()), this, SLOT(clickedMaximizeButton()));
}

void QtCodeArea::lineNumberAreaPaintEvent(QPaintEvent *event)
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

int QtCodeArea::lineNumberDigits() const
{
	int digits = 1;
	int max = qMax(1, int(m_startLineNumber) + blockCount());

	while (max >= 10)
	{
		max /= 10;
		digits++;
	}
	return digits;
}

int QtCodeArea::lineNumberAreaWidth() const
{
	return fontMetrics().width(QLatin1Char('9')) * m_digits + 30;
}

void QtCodeArea::updateLineNumberAreaWidthForDigits(int digits)
{
	m_digits = digits;
	updateLineNumberAreaWidth(0);
}

void QtCodeArea::update()
{
	annotateText();
}

void QtCodeArea::resizeEvent(QResizeEvent *e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void QtCodeArea::showEvent(QShowEvent* event)
{
	int tabWidth = ApplicationSettings::getInstance()->getCodeTabWidth();
	setTabStopWidth(tabWidth * fontMetrics().width('9'));

	setMaximumHeight(sizeHint().height());
}

void QtCodeArea::enterEvent(QEvent* event)
{
	if (m_maximizeButton)
	{
		m_maximizeButton->setEnabled(true);
	}
}

void QtCodeArea::leaveEvent(QEvent* event)
{
	if (m_maximizeButton)
	{
		m_maximizeButton->setEnabled(false);
	}

	if(m_isFocused)
	{
		MessageFocusOut(m_focusedTokenId).dispatch();
	}
	m_isFocused = false;

	annotateText();
}

void QtCodeArea::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		QTextCursor cursor = this->cursorForPosition(event->pos());
		const Annotation* annotation = findAnnotationForPosition(cursor.position());

		if (annotation)
		{
			MessageActivateTokenLocation(annotation->locationId).dispatch();
		}
	}
}

void QtCodeArea::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && m_maximizeButton)
	{
		clickedMaximizeButton();
	}
}

void QtCodeArea::mouseMoveEvent(QMouseEvent* event)
{
	QTextCursor cursor = this->cursorForPosition(event->pos());
	const Annotation* annotation = findAnnotationForPosition(cursor.position());

	if (annotation && annotation->isScope)
	{
		annotation = nullptr;
		MessageFocusOut(m_focusedTokenId).dispatch();
	}

	if (annotation && annotation->tokenId != m_focusedTokenId)
	{
		if(m_isFocused)
		{
			MessageFocusOut(m_focusedTokenId).dispatch();
		}

		m_focusedTokenId = annotation->tokenId;
		m_isFocused = true;
		MessageFocusIn(m_focusedTokenId).dispatch();

		const std::vector<std::string>& errorMessages = m_parent->getErrorMessages();

		if (annotation && errorMessages.size() > annotation->tokenId)
		{
			QToolTip::showText(event->globalPos(), QString::fromStdString(m_parent->getErrorMessages()[annotation->tokenId]));
		}
		else
		{
			QToolTip::hideText();
		}

		annotateText();
	}
}

void QtCodeArea::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void QtCodeArea::updateLineNumberArea(const QRect &rect, int dy)
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

void QtCodeArea::clickedMaximizeButton()
{
	MessageShowFile(m_parent->getFilePath().absoluteStr(), m_startLineNumber, m_startLineNumber + blockCount() - 1).dispatch();
}

void QtCodeArea::clearSelection()
{
	QTextCursor cursor = textCursor();
	cursor.clearSelection();
	setTextCursor(cursor);
}

const QtCodeArea::Annotation* QtCodeArea::findAnnotationForPosition(int pos) const
{
	const Annotation* annotationPtr = nullptr;
	int diff = endTextEditPosition() + 1;

	for (const Annotation& annotation : m_annotations)
	{
		if (pos >= annotation.start && pos <= annotation.end)
		{
			int d = annotation.end - annotation.start;
			if (d < diff)
			{
				diff = d;
				annotationPtr = &annotation;
			}
		}
	}

	return annotationPtr;
}

void QtCodeArea::createAnnotations(std::shared_ptr<TokenLocationFile> locationFile)
{
	locationFile->forEachStartTokenLocation(
		[&](TokenLocation* startLocation)
		{
			Annotation annotation;
			int endLineNumber = m_startLineNumber + blockCount() - 1;
			if (startLocation->getLineNumber() <= endLineNumber)
			{
				if (startLocation->getLineNumber() < m_startLineNumber)
				{
					annotation.start = startTextEditPosition();
				}
				else
				{
					annotation.start = toTextEditPosition(startLocation->getLineNumber(), startLocation->getColumnNumber() - 1);
				}
			}
			else
			{
				return;
			}

			TokenLocation* endLocation = startLocation->getEndTokenLocation();
			if (endLocation->getLineNumber() >= m_startLineNumber)
			{
				if (endLocation->getLineNumber() > endLineNumber)
				{
					annotation.end = endTextEditPosition();
				}
				else
				{
					annotation.end = toTextEditPosition(endLocation->getLineNumber(), endLocation->getColumnNumber());
				}
			}
			else
			{
				return;
			}

			annotation.tokenId = startLocation->getTokenId();
			annotation.locationId = startLocation->getId();
			annotation.isScope = (startLocation->getType() == TokenLocation::LOCATION_SCOPE);
			m_annotations.push_back(annotation);
		}
	);
}

void QtCodeArea::annotateText()
{
	Colori color;
	const std::vector<Id>& ids = m_parent->getActiveTokenIds();
	const std::vector<std::string>& errorMessages = m_parent->getErrorMessages();
	QList<QTextEdit::ExtraSelection> extraSelections;

	for (const Annotation& annotation: m_annotations)
	{
		bool isActive = std::find(ids.begin(), ids.end(), annotation.tokenId) != ids.end();

		if (annotation.tokenId == m_focusedTokenId && errorMessages.size())
		{
			color = Colori(255, 0, 0, 128);
		}
		else if(annotation.tokenId == m_focusedTokenId)
		{
			color = ApplicationSettings::getInstance()->getCodeActiveLinkColor();
		}
		else if (errorMessages.size())
		{
			color = Colori(255, 0, 0, 255);
		}
		else if (isActive)
		{
			color = ApplicationSettings::getInstance()->getCodeActiveLinkColor();

			if (annotation.isScope)
			{
				color.a /= 2;
			}
		}
		else if (annotation.isScope)
		{
			color = ApplicationSettings::getInstance()->getCodeScopeColor();
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

	setExtraSelections(extraSelections);
}

int QtCodeArea::toTextEditPosition(int lineNumber, int columnNumber) const
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

int QtCodeArea::startTextEditPosition() const
{
	return 0;
}

int QtCodeArea::endTextEditPosition() const
{
	int position = 0;

	for (int i = 0; i < document()->blockCount(); i++)
	{
		position += document()->findBlockByLineNumber(i).length();
	}

	return position - 1;
}

void QtCodeArea::focusToken(Id tokenId)
{
	m_focusedTokenId = tokenId;
	annotateText();
}

void QtCodeArea::defocusToken()
{
	m_focusedTokenId = -1;
	annotateText();
}
