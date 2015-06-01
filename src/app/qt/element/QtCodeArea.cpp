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
#include "qt/element/QtCodeSnippet.h"
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
	QtCodeFile* file,
	QtCodeSnippet* parent
)
	: QPlainTextEdit(parent)
	, m_fileWidget(file)
	, m_startLineNumber(startLineNumber)
	, m_hoveredAnnotation(nullptr)
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
	int height = (document()->size().height() + 0.7f) * fontMetrics().lineSpacing() - 3;
	return QSize(width, height);
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

void QtCodeArea::updateContent()
{
	annotateText();
}

bool QtCodeArea::isActive() const
{
	const std::vector<Id>& ids = m_fileWidget->getActiveTokenIds();

	for (const Annotation& annotation: m_annotations)
	{
		if (std::find(ids.begin(), ids.end(), annotation.tokenId) != ids.end())
		{
			return true;
		}
	}

	return false;
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

void QtCodeArea::paintEvent(QPaintEvent* event)
{
	QPainter painter(viewport());

	QTextBlock block = firstVisibleBlock();
	int top = blockBoundingGeometry(block).translated(contentOffset()).top();
	int blockHeight = blockBoundingRect(block).height();

	QColor qColor;

	for (const ScopeAnnotation& scope : m_scopeAnnotations)
	{
		if (scope.isFocused || scope.startLine == scope.endLine)
		{
			qColor = QColor("#90E4EEF2");
		}
		else
		{
			qColor = QColor("#60E4EEF2");
		}

		painter.fillRect(
			0, top + scope.startLine * blockHeight,
			width(), (scope.endLine - scope.startLine + 1) * blockHeight,
			qColor
		);
	}

	QPlainTextEdit::paintEvent(event);
}

void QtCodeArea::enterEvent(QEvent* event)
{
}

void QtCodeArea::leaveEvent(QEvent* event)
{
	setHoveredAnnotation(nullptr);
}

void QtCodeArea::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		QTextCursor cursor = this->cursorForPosition(event->pos());
		const Annotation* annotation = findAnnotationForPosition(cursor.position());

		if (annotation && !annotation->isScope)
		{
			MessageActivateTokenLocation(annotation->locationId).dispatch();
		}
	}
}

void QtCodeArea::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		MessageShowFile(m_fileWidget->getFilePath().absoluteStr(), m_startLineNumber, m_startLineNumber + blockCount() - 1).dispatch();
	}
}

void QtCodeArea::mouseMoveEvent(QMouseEvent* event)
{
	QTextCursor cursor = this->cursorForPosition(event->pos());
	const Annotation* annotation = findAnnotationForPosition(cursor.position());

	if (annotation && annotation->isScope)
	{
		annotation = nullptr;
	}

	if (annotation != m_hoveredAnnotation)
	{
		setHoveredAnnotation(annotation);

		const std::vector<std::string>& errorMessages = m_fileWidget->getErrorMessages();

		if (annotation && errorMessages.size() > annotation->tokenId)
		{
			QToolTip::showText(event->globalPos(), QString::fromStdString(m_fileWidget->getErrorMessages()[annotation->tokenId]));
		}
		else
		{
			QToolTip::hideText();
		}
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

void QtCodeArea::clearSelection()
{
	QTextCursor cursor = textCursor();
	cursor.clearSelection();
	setTextCursor(cursor);
}

QtCodeArea::ScopeAnnotation::ScopeAnnotation()
	: startLine(0)
	, endLine(0)
	, tokenId(0)
	, isFocused(false)
{
}

bool QtCodeArea::ScopeAnnotation::operator!=(const ScopeAnnotation& other) const
{
	return (startLine != other.startLine || endLine != other.endLine || tokenId != other.tokenId || isFocused != other.isFocused);
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
			unsigned int endLineNumber = m_startLineNumber + blockCount() - 1;
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
	Id focusedTokenId = m_fileWidget->getFocusedTokenId();
	const std::vector<Id>& ids = m_fileWidget->getActiveTokenIds();
	const std::vector<std::string>& errorMessages = m_fileWidget->getErrorMessages();

	std::vector<ScopeAnnotation> scopeAnnotations;

	QColor color;
	QList<QTextEdit::ExtraSelection> extraSelections;

	for (const Annotation& annotation: m_annotations)
	{
		bool isActive = std::find(ids.begin(), ids.end(), annotation.tokenId) != ids.end();
		bool isFocused = (annotation.tokenId == focusedTokenId);

		if (&annotation == m_hoveredAnnotation && errorMessages.size())
		{
			color.setNamedColor("#80FF0000");
		}
		else if (errorMessages.size())
		{
			color.setNamedColor("#FFFF0000");
		}
		else if (isActive || isFocused)
		{
			color.setNamedColor("#90B6D1DD");
		}
		else
		{
			color.setNamedColor("#90EBEBEB");
		}

		QTextEdit::ExtraSelection selection;
		selection.format.setBackground(color);

		ScopeAnnotation scopeAnnotation;

		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		selection.cursor.setPosition(annotation.start);
		scopeAnnotation.startLine = selection.cursor.blockNumber();
		selection.cursor.setPosition(annotation.end, QTextCursor::KeepAnchor);
		scopeAnnotation.endLine = selection.cursor.blockNumber();

		if (annotation.isScope || (isActive && ids.size() == 1))
		{
			if (isActive || isFocused)
			{
				scopeAnnotation.tokenId = annotation.tokenId;
				if (!isActive)
				{
					scopeAnnotation.isFocused = isFocused;
				}
				scopeAnnotations.push_back(scopeAnnotation);
			}
		}

		if (!annotation.isScope || (isActive && ids.size() == 1 && scopeAnnotation.startLine == scopeAnnotation.endLine))
		{
			extraSelections.append(selection);
		}
	}

	setExtraSelections(extraSelections);

	bool needsUpdate = false;
	if (scopeAnnotations.size() != m_scopeAnnotations.size())
	{
		needsUpdate = true;
	}
	else
	{
		for (size_t i = 0; i < scopeAnnotations.size(); i++)
		{
			if (scopeAnnotations[i] != m_scopeAnnotations[i])
			{
				needsUpdate = true;
				break;
			}
		}
	}

	m_scopeAnnotations = scopeAnnotations;

	if (needsUpdate)
	{
		viewport()->update();
	}
}

void QtCodeArea::setHoveredAnnotation(const Annotation* annotation)
{
	if (m_hoveredAnnotation)
	{
		MessageFocusOut(m_hoveredAnnotation->tokenId).dispatch();
	}

	m_hoveredAnnotation = annotation;

	if (annotation)
	{
		MessageFocusIn(annotation->tokenId).dispatch();
	}
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
