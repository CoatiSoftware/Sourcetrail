#include "qt/element/QtCodeArea.h"

#include <QApplication>
#include <QFont>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
#include <QToolTip>

#include "utility/messaging/type/MessageActivateLocalSymbols.h"
#include "utility/messaging/type/MessageActivateTokenLocations.h"
#include "utility/messaging/type/MessageActivateTokenIds.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageMoveIDECursor.h"
#include "utility/utility.h"

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeNavigator.h"
#include "qt/utility/QtContextMenu.h"
#include "qt/utility/QtHighlighter.h"
#include "settings/ApplicationSettings.h"
#include "settings/ColorScheme.h"

std::vector<QtCodeArea::AnnotationColor> QtCodeArea::s_annotationColors;

MouseWheelOverScrollbarFilter::MouseWheelOverScrollbarFilter(QObject* parent)
	: QObject(parent)
{
}

bool MouseWheelOverScrollbarFilter::eventFilter(QObject* obj, QEvent* event)
{
	QScrollBar* scrollbar = dynamic_cast<QScrollBar*>(obj);
	if (event->type() == QEvent::Wheel && scrollbar)
	{
		QRect scrollbarArea(scrollbar->pos(), scrollbar->size());
		QPoint globalMousePos = dynamic_cast<QWheelEvent*>(event)->globalPos();
		QPoint localMousePos = scrollbar->mapFromGlobal(globalMousePos);

		// instead of "scrollbar->underMouse()" we need this check implemented here because "underMouse()"
		// does not work when the mouse enters the area without being moved
		if (scrollbarArea.contains(localMousePos))
		{
			event->ignore();
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}

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

void QtCodeArea::clearAnnotationColors()
{
	s_annotationColors.clear();
}

QtCodeArea::QtCodeArea(
	uint startLineNumber,
	const std::string& code,
	std::shared_ptr<TokenLocationFile> locationFile,
	QtCodeNavigator* navigator,
	QWidget* parent
)
	: QPlainTextEdit(parent)
	, m_navigator(navigator)
	, m_startLineNumber(startLineNumber)
	, m_code(code)
	, m_locationFile(locationFile)
	, m_digits(0)
	, m_isSelecting(false)
	, m_isPanning(false)
	, m_setIDECursorPositionAction(nullptr)
	, m_eventPosition(0, 0)
	, m_isActiveFile(false)
	, m_lineNumbersHidden(false)
	, m_wasAnnotated(false)
	, m_endTextEditPosition(0)
{
	setObjectName("code_area");
	setReadOnly(true);
	setFrameStyle(QFrame::NoFrame);
	setLineWrapMode(QPlainTextEdit::NoWrap);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	viewport()->setCursor(Qt::ArrowCursor);

	m_lineNumberArea = new LineNumberArea(this);
	m_highlighter = new QtHighlighter(document());

	std::string displayCode = m_code;
	if (!displayCode.empty() && *displayCode.rbegin() == '\n')
	{
		displayCode.pop_back();
	}

	setPlainText(QString::fromUtf8(displayCode.c_str()));
	createLineLengthCache();

	m_digits = lineNumberDigits();
	updateLineNumberAreaWidth();

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));

	this->setMouseTracking(true);

	// MouseWheelOverScrollbarFilter is deleted by parent.
	horizontalScrollBar()->installEventFilter(new MouseWheelOverScrollbarFilter(this));
	m_scrollSpeedChangeListener.setScrollBar(horizontalScrollBar());

	createActions();

	m_highlighter->highlightDocument();

	createAnnotations(locationFile);
	annotateText();
}

QtCodeArea::~QtCodeArea()
{
	if (m_setIDECursorPositionAction != nullptr)
	{
		m_setIDECursorPositionAction->disconnect();
		delete m_setIDECursorPositionAction;
	}
}

QSize QtCodeArea::sizeHint() const
{
	QTextBlock block = firstVisibleBlock();
	float height = blockBoundingGeometry(block).translated(contentOffset()).top();

	while (block.isValid())
	{
		height += blockBoundingRect(block).height();
		block = block.next();
	}

	if (horizontalScrollBar()->isVisible())
	{
		height += horizontalScrollBar()->height();
	}

	return QSize(0, height + 5);
}

uint QtCodeArea::getStartLineNumber() const
{
	return m_startLineNumber;
}

uint QtCodeArea::getEndLineNumber() const
{
	return m_startLineNumber + blockCount() - 1;
}

std::shared_ptr<TokenLocationFile> QtCodeArea::getTokenLocationFile() const
{
	return m_locationFile;
}

void QtCodeArea::lineNumberAreaPaintEvent(QPaintEvent *event)
{
	QPainter painter(m_lineNumberArea);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom = top + static_cast<int>(blockBoundingRect(block).height());

	std::set<int> activeLineNumbers = getActiveLineNumbers();

	ColorScheme* scheme = ColorScheme::getInstance().get();

	QColor textColor(scheme->getColor("code/snippet/line_number/text").c_str());
	QColor inactiveTextColor(scheme->getColor("code/snippet/line_number/inactive_text").c_str());
	QColor markerColor(scheme->getColor("code/snippet/line_number/marker").c_str());

	QPen p = painter.pen();

	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			int number = blockNumber + m_startLineNumber;

			p.setColor(textColor);

			if (activeLineNumbers.find(number) != activeLineNumbers.end())
			{
				painter.fillRect(m_lineNumberArea->width() - 8, top, 3, fontMetrics().height() + 1, markerColor);
			}
			else if (!m_isActiveFile)
			{
				p.setColor(inactiveTextColor);
			}

			painter.setPen(p);
			painter.drawText(0, top, m_lineNumberArea->width() - 16, fontMetrics().height(), Qt::AlignRight, QString::number(number));
		}

		block = block.next();
		top = bottom;
		bottom = top + static_cast<int>(blockBoundingRect(block).height());
		blockNumber++;
	}
}

int QtCodeArea::lineNumberDigits() const
{
	int max = qMax(1, int(m_startLineNumber) + blockCount());
	return utility::digits(max);
}

int QtCodeArea::lineNumberAreaWidth() const
{
	if (!m_lineNumbersHidden)
	{
		return fontMetrics().width(QLatin1Char('9')) * m_digits + 30;
	}

	return 0;
}

void QtCodeArea::updateLineNumberAreaWidthForDigits(int digits)
{
	m_digits = digits;
	updateLineNumberAreaWidth();
}

void QtCodeArea::updateContent()
{
	annotateText();
}

void QtCodeArea::setIsActiveFile(bool isActiveFile)
{
	m_isActiveFile = isActiveFile;
}

uint QtCodeArea::getLineNumberForLocationId(Id locationId) const
{
	for (const Annotation& annotation : m_annotations)
	{
		if (annotation.locationId == locationId)
		{
			return annotation.startLine;
		}
	}

	return 0;
}

QRectF QtCodeArea::getLineRectForLineNumber(uint lineNumber) const
{
	if (lineNumber < getStartLineNumber())
	{
		lineNumber = getStartLineNumber();
	}
	else if (lineNumber > getEndLineNumber())
	{
		lineNumber = getEndLineNumber();
	}

	QTextBlock block = document()->findBlockByLineNumber(lineNumber - m_startLineNumber);
	return blockBoundingGeometry(block);
}

std::string QtCodeArea::getCode() const
{
	return m_code;
}

void QtCodeArea::hideLineNumbers()
{
	m_lineNumberArea->hide();
	m_lineNumbersHidden = true;
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

	setFixedHeight(sizeHint().height());
}

void QtCodeArea::paintEvent(QPaintEvent* event)
{
	QPainter painter(viewport());

	QTextBlock block = firstVisibleBlock();
	int top = blockBoundingGeometry(block).translated(contentOffset()).top();
	int bottom = top + blockBoundingRect(block).height();
	int blockHeight = blockBoundingRect(block).height();

	int firstVisibleLine = -1;
	int lastVisibleLine = -1;
	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible())
		{
			if (firstVisibleLine < 0 && bottom >= event->rect().top())
			{
				firstVisibleLine = block.blockNumber();;
			}
			lastVisibleLine = block.blockNumber();;
		}

		block = block.next();
		top = bottom;
		bottom = top + static_cast<int>(blockBoundingRect(block).height());
	}
	firstVisibleLine += m_startLineNumber;
	lastVisibleLine += m_startLineNumber;

	int borderRadius = 3;

	for (const Annotation& annotation : m_annotations)
	{
		if (annotation.startLine > lastVisibleLine || annotation.endLine < firstVisibleLine)
		{
			continue;
		}

		const AnnotationColor& color = getAnnotationColorForAnnotation(annotation);

		if (color.border == "transparent" && color.fill == "transparent")
		{
			continue;
		}

		painter.setPen(QPen(color.border.c_str()));
		painter.setBrush(QBrush(color.fill.c_str()));

		if (annotation.locationType == LOCATION_SCOPE)
		{
			painter.drawRoundedRect(
				0, top + (annotation.startLine - m_startLineNumber) * blockHeight,
				width(), (annotation.endLine - annotation.startLine + 1) * blockHeight,
				borderRadius, borderRadius
			);
		}
		else
		{
			std::vector<QRect> rects = getCursorRectsForAnnotation(annotation);
			for (QRect rect : rects)
			{
				rect.adjust(-1, 0, 1, 1);
				painter.drawRoundedRect(rect, borderRadius, borderRadius);
			}
		}
	}

	QPlainTextEdit::paintEvent(event);
}

void QtCodeArea::enterEvent(QEvent* event)
{
}

void QtCodeArea::leaveEvent(QEvent* event)
{
	setHoveredAnnotations(std::vector<const Annotation*>());
}

void QtCodeArea::mousePressEvent(QMouseEvent* event)
{
	clearSelection();

	if (event->button() == Qt::LeftButton)
	{
		if (Qt::KeyboardModifier::ShiftModifier & QApplication::keyboardModifiers())
		{
			m_isSelecting = true;
			QTextCursor cursor = this->cursorForPosition(event->pos());
			setTextCursor(cursor);
		}
		else
		{
			m_isPanning = true;
			m_oldMousePosition = event->pos();
			m_panningDistance = 0;
		}
	}
}

void QtCodeArea::mouseReleaseEvent(QMouseEvent* event)
{
	const int panningThreshold = 5;
	if (event->button() == Qt::LeftButton)
	{
		if (m_isSelecting)
		{
			m_isSelecting = false;
			return;
		}

		m_isPanning = false;

		if (m_panningDistance < panningThreshold) // dont do anything if mouse is release to end some real panning action.
		{
			if (Qt::KeyboardModifier::ControlModifier & QApplication::keyboardModifiers())
			{
				m_eventPosition = event->pos();
				setIDECursorPosition();
			}
			else
			{
				QTextCursor cursor = this->cursorForPosition(event->pos());
				std::vector<const Annotation*> annotations = getInteractiveAnnotationsForPosition(cursor.position());

				activateTokenLocations(annotations);
				activateLocalSymbols(annotations);
			}
		}
	}
}

void QtCodeArea::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isSelecting)
	{
		QTextCursor cursor = textCursor();
		cursor.setPosition(this->cursorForPosition(event->pos()).position(), QTextCursor::KeepAnchor);
		setTextCursor(cursor);
	}
	else if (m_isPanning)
	{
		const QPoint currentMousePosition = event->pos();
		const int deltaX = currentMousePosition.x() - m_oldMousePosition.x();
		const int deltaY = currentMousePosition.y() - m_oldMousePosition.y();
		m_oldMousePosition = currentMousePosition;

		QScrollBar* scrollbar = horizontalScrollBar();
		int visibleContentWidth = width() - lineNumberAreaWidth();
		float deltaPosRatio = float(deltaX) / (visibleContentWidth);
		scrollbar->setValue(scrollbar->value() - utility::roundToInt(deltaPosRatio * scrollbar->pageStep()));

		m_panningDistance += abs(deltaX + deltaY);
	}

	QTextCursor cursor = this->cursorForPosition(event->pos());
	std::vector<const Annotation*> annotations = getInteractiveAnnotationsForPosition(cursor.position());

	bool same = annotations.size() == m_hoveredAnnotations.size();
	if (same)
	{
		for (size_t i = 0; i < annotations.size(); i++)
		{
			if (annotations[i] != m_hoveredAnnotations[i])
			{
				same = false;
				break;
			}
		}
	}

	if (!same)
	{
		QToolTip::hideText();

		setHoveredAnnotations(annotations);

		if (annotations.size() == 1)
		{
			std::string errorMessage = m_navigator->getErrorMessageForId(annotations[0]->tokenId);
			QToolTip::showText(event->globalPos(), QString::fromStdString(errorMessage));
		}
	}
}

void QtCodeArea::contextMenuEvent(QContextMenuEvent* event)
{
	if (m_setIDECursorPositionAction != nullptr)
	{
		m_eventPosition = event->pos();
		QtContextMenu::getInstance()->showExtended(event, this, std::vector<QAction*>(1, m_setIDECursorPositionAction));
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
		updateLineNumberAreaWidth();
	}
}

void QtCodeArea::clearSelection()
{
	QTextCursor cursor = textCursor();
	cursor.clearSelection();

	QScrollBar* scrollbar = horizontalScrollBar();
	int scrollValue = horizontalScrollBar()->value();

	setTextCursor(cursor);

	scrollbar->setValue(scrollValue);
}

void QtCodeArea::setIDECursorPosition()
{
	std::pair<int, int> lineColumn = toLineColumn(this->cursorForPosition(m_eventPosition).position());

	MessageMoveIDECursor(m_locationFile->getFilePath().str(), lineColumn.first, lineColumn.second).dispatch();
}

std::vector<const QtCodeArea::Annotation*> QtCodeArea::getInteractiveAnnotationsForPosition(int pos) const
{
	std::vector<const QtCodeArea::Annotation*> annotations;

	for (const Annotation& annotation : m_annotations)
	{
		const LocationType& type = annotation.locationType;
		if ((type == LOCATION_TOKEN || type == LOCATION_LOCAL_SYMBOL || type == LOCATION_ERROR)
			&& pos >= annotation.start && pos <= annotation.end)
		{
			annotations.push_back(&annotation);
		}
	}

	return annotations;
}

void QtCodeArea::activateTokenLocations(const std::vector<const Annotation*>& annotations)
{
	std::vector<Id> tokenLocationIds;
	std::set<Id> tokenIds;

	bool allActive = true;
	for (const Annotation* annotation : annotations)
	{
		if (annotation->locationType == LOCATION_TOKEN)
		{
			if (!annotation->isActive)
			{
				allActive = false;
			}

			if (annotation->locationId > 0)
			{
				tokenLocationIds.push_back(annotation->locationId);
			}
			if (annotation->tokenId > 0)
			{
				tokenIds.insert(annotation->tokenId);
			}
		}
	}

	if (!allActive)
	{
		if (tokenLocationIds.size())
		{
			MessageActivateTokenLocations(tokenLocationIds).dispatch();
		}
		else if (tokenIds.size()) // fallback for links in project description
		{
			MessageActivateTokenIds(utility::toVector(tokenIds)).dispatch();
		}
	}
}

void QtCodeArea::activateLocalSymbols(const std::vector<const Annotation*>& annotations)
{
	std::vector<Id> localSymbolIds;

	bool allActive = true;
	for (const Annotation* annotation : annotations)
	{
		if (annotation->locationType == LOCATION_LOCAL_SYMBOL)
		{
			if (!annotation->isActive)
			{
				allActive = false;
			}

			if (annotation->tokenId > 0)
			{
				localSymbolIds.push_back(annotation->tokenId);
			}
		}
	}

	if (!allActive || !localSymbolIds.size())
	{
		MessageActivateLocalSymbols(localSymbolIds).dispatch();
	}
}

void QtCodeArea::createAnnotations(std::shared_ptr<TokenLocationFile> locationFile)
{
	locationFile->forEachStartTokenLocation(
		[&](TokenLocation* startLocation)
		{
			Annotation annotation;
			uint endLineNumber = getEndLineNumber();
			if (startLocation->getLineNumber() <= endLineNumber)
			{
				if (startLocation->getLineNumber() < m_startLineNumber)
				{
					annotation.start = startTextEditPosition();
					annotation.startLine = m_startLineNumber;
					annotation.startCol = 0;
				}
				else
				{
					annotation.start = toTextEditPosition(startLocation->getLineNumber(), startLocation->getColumnNumber() - 1);
					annotation.startLine = startLocation->getLineNumber();
					annotation.startCol = startLocation->getColumnNumber() - 1;
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
					annotation.endLine = endLineNumber;
					annotation.endCol = m_lineLengths[document()->blockCount() - 1];
				}
				else
				{
					annotation.end = toTextEditPosition(endLocation->getLineNumber(), endLocation->getColumnNumber());
					annotation.endLine = endLocation->getLineNumber();
					annotation.endCol = endLocation->getColumnNumber();
				}
			}
			else
			{
				return;
			}

			annotation.tokenId = startLocation->getTokenId();
			annotation.locationId = startLocation->getId();

			annotation.locationType = startLocation->getType();

			annotation.isActive = false;
			annotation.isFocused = false;

			m_annotations.push_back(annotation);
		}
	);
}

void QtCodeArea::annotateText()
{
	const std::vector<Id>& currentActiveTokenIds = m_navigator->getCurrentActiveTokenIds();
	const std::vector<Id>& currentActiveLocationIds = m_navigator->getCurrentActiveLocationIds();

	const std::vector<Id>& activeTokenIds = m_navigator->getActiveTokenIds();
	const std::vector<Id>& activeLocalSymbolIds = m_navigator->getActiveLocalSymbolIds();
	const std::vector<Id>& focusIds = m_navigator->getFocusedTokenIds();

	bool needsUpdate = false;
	for (Annotation& annotation: m_annotations)
	{
		bool wasActive = annotation.isActive;
		bool wasFocused = annotation.isFocused;
		const AnnotationColor& oldColor = getAnnotationColorForAnnotation(annotation);

		annotation.isActive = (
			std::find(currentActiveTokenIds.begin(), currentActiveTokenIds.end(), annotation.tokenId) != currentActiveTokenIds.end() ||
			std::find(currentActiveLocationIds.begin(), currentActiveLocationIds.end(), annotation.locationId) != currentActiveLocationIds.end() ||
			std::find(activeLocalSymbolIds.begin(), activeLocalSymbolIds.end(), annotation.tokenId) != activeLocalSymbolIds.end()
		);
		if (!annotation.isActive)
		{
			annotation.isFocused = (
				std::find(focusIds.begin(), focusIds.end(), annotation.tokenId) != focusIds.end() ||
				std::find(activeTokenIds.begin(), activeTokenIds.end(), annotation.tokenId) != activeTokenIds.end()
			);
		}

		const AnnotationColor& newColor = getAnnotationColorForAnnotation(annotation);
		if ((newColor.text != oldColor.text || !m_wasAnnotated))
		{
			if (newColor.text.size() > 0 && newColor.text != "transparent")
			{
				if (!annotation.oldTextColor.isValid())
				{
					annotation.oldTextColor = m_highlighter->getFormat(annotation.start, annotation.end).foreground().color();
				}

				setTextColorForAnnotation(annotation, QColor(newColor.text.c_str()));
			}
			else if (annotation.oldTextColor.isValid())
			{
				setTextColorForAnnotation(annotation, annotation.oldTextColor);
				annotation.oldTextColor = QColor();
			}
		}

		if (wasFocused != annotation.isFocused || wasActive != annotation.isActive)
		{
			needsUpdate = true;
		}
	}

	if (needsUpdate)
	{
		m_lineNumberArea->update();
		viewport()->update();
	}

	m_wasAnnotated = true;
}

void QtCodeArea::setHoveredAnnotations(const std::vector<const Annotation*>& annotations)
{
	if (m_hoveredAnnotations.size())
	{
		std::vector<Id> tokenIds;
		for (const Annotation* annotation : m_hoveredAnnotations)
		{
			tokenIds.push_back(annotation->tokenId);
		}

		MessageFocusOut(tokenIds).dispatch();
	}

	m_hoveredAnnotations = annotations;

	if (annotations.size())
	{
		std::vector<Id> tokenIds;
		for (const Annotation* annotation : annotations)
		{
			tokenIds.push_back(annotation->tokenId);
		}

		MessageFocusIn(tokenIds).dispatch();
	}
}

int QtCodeArea::toTextEditPosition(int lineNumber, int columnNumber) const
{
	lineNumber -= m_startLineNumber - 1;
	int position = 0;

	for (int i = 0; i < lineNumber - 1; i++)
	{
		position += m_lineLengths[i];
	}

	position += columnNumber;
	return position;
}

std::pair<int, int> QtCodeArea::toLineColumn(int textEditPosition) const
{
	int lineNumber = m_startLineNumber;
	for (int i = 0; i < document()->lineCount(); i++)
	{
		int nextTextEditPosition = textEditPosition - m_lineLengths[i];
		if (nextTextEditPosition >= 0)
		{
			textEditPosition = nextTextEditPosition;
			lineNumber++;
		}
		else
		{
			break;
		}
	}
	return std::make_pair(lineNumber, textEditPosition);
}

int QtCodeArea::startTextEditPosition() const
{
	return 0;
}

int QtCodeArea::endTextEditPosition() const
{
	return m_endTextEditPosition;
}

std::set<int> QtCodeArea::getActiveLineNumbers() const
{
	std::set<int> activeLineNumbers;

	for (const Annotation& annotation : m_annotations)
	{
		if (annotation.isActive || annotation.isFocused)
		{
			for (int i = annotation.startLine; i <= annotation.endLine; i++)
			{
				activeLineNumbers.insert(i);
			}
		}
	}

	return activeLineNumbers;
}

std::vector<QRect> QtCodeArea::getCursorRectsForAnnotation(const Annotation& annotation) const
{
	std::vector<QRect> rects;

	QTextCursor cursor = QTextCursor(document());
	cursor.setPosition(annotation.start);
	QRect rectStart = cursorRect(cursor);
	QRect rectEnd;

	int line = annotation.startLine;
	while (line <= annotation.endLine)
	{
		if (line == annotation.endLine)
		{
			// Avoid that annotations at line end span down to first column of the next line.
			if (annotation.startLine != annotation.endLine ||
				m_lineLengths[line - m_startLineNumber] != annotation.endCol)
			{
				cursor.setPosition(annotation.end);
			}
		}
		else
		{
			cursor.setPosition(toTextEditPosition(line, m_lineLengths[line - m_startLineNumber] - 1));
		}

		rectEnd = cursorRect(cursor);
		rects.push_back(QRect(
			rectStart.left(),
			rectStart.top(),
			rectEnd.right() - rectStart.left(),
			rectEnd.bottom() - rectStart.top()
		));

		line++;

		if (int(line - m_startLineNumber) < document()->blockCount())
		{
			cursor.setPosition(toTextEditPosition(line, 0));
			rectStart = cursorRect(cursor);
		}
	}

	return rects;
}

const QtCodeArea::AnnotationColor& QtCodeArea::getAnnotationColorForAnnotation(const Annotation& annotation)
{
	if (!s_annotationColors.size())
	{
		ColorScheme* scheme = ColorScheme::getInstance().get();
		std::vector<std::string> types = { "token", "local_symbol", "scope", "error", "fulltext" };
		std::vector<ColorScheme::ColorState> states = { ColorScheme::NORMAL, ColorScheme::FOCUS, ColorScheme::ACTIVE };

		for (const std::string& type : types)
		{
			for (const ColorScheme::ColorState& state : states)
			{
				AnnotationColor color;
				color.border = scheme->getCodeAnnotationTypeColor(type, "border", state);
				color.fill = scheme->getCodeAnnotationTypeColor(type, "fill", state);
				color.text = scheme->getCodeAnnotationTypeColor(type, "text", state);
				s_annotationColors.push_back(color);
			}
		}
	}

	size_t i = 0;

	if (annotation.locationType == LOCATION_LOCAL_SYMBOL)
	{
		i = 3;
	}
	else if (annotation.locationType == LOCATION_SCOPE)
	{
		i = 6;
	}
	else if (annotation.locationType == LOCATION_ERROR)
	{
		i = 9;
	}
	else if (annotation.locationType == LOCATION_FULLTEXT)
	{
		i = 12;
	}

	if (annotation.isActive)
	{
		i += 2;
	}
	else if (annotation.isFocused)
	{
		i += 1;
	}

	return s_annotationColors[i];
}

void QtCodeArea::setTextColorForAnnotation(Annotation& annotation, QColor color) const
{
	QTextCharFormat format;
	format.setForeground(color);
	m_highlighter->applyFormat(annotation.start, annotation.end, format);
}

void QtCodeArea::createActions()
{
	m_setIDECursorPositionAction = new QAction(tr("Set IDE Cursor"), this);
	m_setIDECursorPositionAction->setStatusTip(tr("Set the IDE Cursor to this code position"));
	m_setIDECursorPositionAction->setToolTip(tr("Set the IDE Cursor to this code position"));
	connect(m_setIDECursorPositionAction, SIGNAL(triggered()), this, SLOT(setIDECursorPosition()));
}

void QtCodeArea::createLineLengthCache()
{
	m_endTextEditPosition = -1;

	m_lineLengths.clear();

	for (QTextBlock it = document()->begin(); it != document()->end(); it = it.next())
	{
		m_lineLengths.push_back(it.length());
		m_endTextEditPosition += it.length();
	}
}
