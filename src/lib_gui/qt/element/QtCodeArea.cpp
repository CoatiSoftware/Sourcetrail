#include "qt/element/QtCodeArea.h"

#include <qapplication.h>
#include <QFont>
#include <QHBoxLayout>
#include <qmenu.h>
#include <QPainter>
#include <QPushButton>
#include <QToolTip>
#include <qscrollbar.h>

#include "utility/messaging/type/MessageActivateLocalSymbols.h"
#include "utility/messaging/type/MessageActivateTokenLocations.h"
#include "utility/messaging/type/MessageActivateTokenIds.h"
#include "utility/messaging/type/MessageFocusIn.h"
#include "utility/messaging/type/MessageFocusOut.h"
#include "utility/messaging/type/MessageMoveIDECursor.h"
#include "utility/utility.h"

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "qt/element/QtCodeFile.h"
#include "qt/element/QtCodeSnippet.h"
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
	QtCodeFile* file,
	QtCodeSnippet* parent
)
	: QPlainTextEdit(parent)
	, m_fileWidget(file)
	, m_startLineNumber(startLineNumber)
	, m_code(code)
	, m_locationFile(locationFile)
	, m_digits(0)
	, m_isPanning(false)
	, m_setIDECursorPositionAction(nullptr)
	, m_eventPosition(0, 0)
	, m_isActiveFile(false)
	, m_lineNumbersHidden(false)
	, m_wasAnnotated(false)
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
	if (*displayCode.rbegin() == '\n')
	{
		displayCode.pop_back();
	}

	setPlainText(QString::fromUtf8(displayCode.c_str()));

	m_digits = lineNumberDigits();
	updateLineNumberAreaWidth();

	connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
	connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
	connect(this, SIGNAL(selectionChanged()), this, SLOT(clearSelection()));

	this->setMouseTracking(true);

	// MouseWheelOverScrollbarFilter is deleted by parent.
	horizontalScrollBar()->installEventFilter(new MouseWheelOverScrollbarFilter(this));

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

	return QSize(0, height + 1);
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

QtCodeFile* QtCodeArea::getFile() const
{
	return m_fileWidget;
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
	QColor backgroundColor(scheme->getColor("code/snippet/line_number/background").c_str());
	backgroundColor.setAlpha(150);

	while (block.isValid() && top <= event->rect().bottom())
	{
		if (block.isVisible() && bottom >= event->rect().top())
		{
			int number = blockNumber + m_startLineNumber;
			painter.drawText(0, top, m_lineNumberArea->width() - 13, fontMetrics().height(), Qt::AlignRight, QString::number(number));

			if (!m_isActiveFile && activeLineNumbers.find(number) == activeLineNumbers.end())
			{
				painter.fillRect(0, top, m_lineNumberArea->width(), fontMetrics().height(), backgroundColor);
			}
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

void QtCodeArea::setIsActiveFile(bool isActiveFile)
{
	m_isActiveFile = isActiveFile;
}

uint QtCodeArea::getFirstActiveLineNumber() const
{
	for (const Annotation& annotation : m_annotations)
	{
		if (annotation.isActive)
		{
			return annotation.startLine;
		}
	}

	return 0;
}

QRectF QtCodeArea::getLineRectForLineNumber(uint lineNumber) const
{
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

	QPainter painter2(viewport());
	std::set<int> activeLineNumbers = getActiveLineNumbers();

	ColorScheme* scheme = ColorScheme::getInstance().get();
	QColor backgroundColor(scheme->getColor("code/snippet/background").c_str());
	backgroundColor.setAlpha(75);

	for (int i = 0; i < document()->blockCount(); i++)
	{
		int lineNumber = i + m_startLineNumber;
		if (!m_isActiveFile && activeLineNumbers.find(lineNumber) == activeLineNumbers.end() &&
			lineNumber >= firstVisibleLine && lineNumber <= lastVisibleLine)
		{
			painter.fillRect(0, top + i * blockHeight, width(), blockHeight, backgroundColor);
		}
	}
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
	if (event->button() == Qt::LeftButton)
	{
		m_isPanning = true;
		m_oldMousePosition = event->pos();
		m_panningDistance = 0;
	}
}

void QtCodeArea::mouseReleaseEvent(QMouseEvent* event)
{
	const int panningThreshold = 5;
	if (event->button() == Qt::LeftButton)
	{
		m_isPanning = false;

		if (m_panningDistance < panningThreshold) // dont do anything if mouse is release to end some real panning action.
		{
			if (Qt::KeyboardModifier::ControlModifier && QApplication::keyboardModifiers())
			{
				m_eventPosition = event->pos();
				setIDECursorPosition();
			}
			else if (!m_fileWidget->hasErrors())
			{
				QTextCursor cursor = this->cursorForPosition(event->pos());
				std::vector<const Annotation*> annotations = getNonScopeAnnotationsForPosition(cursor.position());

				activateTokenLocations(annotations);
				activateLocalSymbols(annotations);
			}
		}
	}
}

void QtCodeArea::mouseMoveEvent(QMouseEvent* event)
{
	if (m_isPanning)
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
	std::vector<const Annotation*> annotations = getNonScopeAnnotationsForPosition(cursor.position());

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

		std::vector<std::string> errorMessages = m_fileWidget->getErrorMessages();
		if (annotations.size() == 1 && errorMessages.size() > annotations[0]->tokenId)
		{
			QToolTip::showText(event->globalPos(), QString::fromStdString(errorMessages[annotations[0]->tokenId]));
		}
	}
}

void QtCodeArea::contextMenuEvent(QContextMenuEvent* event)
{
	if (m_setIDECursorPositionAction != nullptr)
	{
		m_eventPosition = event->pos();

		QMenu menu(this);
		menu.addAction(m_setIDECursorPositionAction);
		menu.exec(event->globalPos());
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
	setTextCursor(cursor);
}

void QtCodeArea::setIDECursorPosition()
{
	std::pair<int, int> lineColumn = toLineColumn(this->cursorForPosition(m_eventPosition).position());

	MessageMoveIDECursor(m_locationFile->getFilePath().str(), lineColumn.first, lineColumn.second).dispatch();
}

std::vector<const QtCodeArea::Annotation*> QtCodeArea::getNonScopeAnnotationsForPosition(int pos) const
{
	std::vector<const QtCodeArea::Annotation*> annotations;

	for (const Annotation& annotation : m_annotations)
	{
		if (annotation.locationType != LOCATION_SCOPE && pos >= annotation.start && pos <= annotation.end)
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
					annotation.endCol = document()->findBlockByLineNumber(document()->blockCount() - 1).length();
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
			annotation.isError = false;

			annotation.isActive = false;
			annotation.isFocused = false;

			m_annotations.push_back(annotation);
		}
	);
}

void QtCodeArea::annotateText()
{
	const std::vector<Id>& activeTokenIds = m_fileWidget->getActiveTokenIds();
	const std::vector<Id>& activeLocalSymbolIds = m_fileWidget->getActiveLocalSymbolIds();
	const std::vector<Id>& focusIds = m_fileWidget->getFocusedTokenIds();

	bool isError = m_fileWidget->hasErrors();

	bool needsUpdate = false;
	for (Annotation& annotation: m_annotations)
	{
		bool wasActive = annotation.isActive;
		bool wasFocused = annotation.isFocused;
		const AnnotationColor& oldColor = getAnnotationColorForAnnotation(annotation);

		annotation.isActive = (
			std::find(activeTokenIds.begin(), activeTokenIds.end(), annotation.tokenId) != activeTokenIds.end() ||
			std::find(activeLocalSymbolIds.begin(), activeLocalSymbolIds.end(), annotation.tokenId) != activeLocalSymbolIds.end()
		);
		annotation.isFocused = std::find(focusIds.begin(), focusIds.end(), annotation.tokenId) != focusIds.end();

		annotation.isError = isError;

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
		position += document()->findBlockByLineNumber(i).length();
	}

	position += columnNumber;
	return position;
}

std::pair<int, int> QtCodeArea::toLineColumn(int textEditPosition) const
{
	int lineNumber = m_startLineNumber;
	for (int i = 0; i < document()->lineCount(); i++)
	{
		int nextTextEditPosition = textEditPosition - document()->findBlockByLineNumber(i).length();
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
	int position = 0;

	for (int i = 0; i < document()->blockCount(); i++)
	{
		position += document()->findBlockByLineNumber(i).length();
	}

	return position - 1;
}

std::set<int> QtCodeArea::getActiveLineNumbers() const
{
	std::set<int> activeLineNumbers;

	if (m_isActiveFile)
	{
		return activeLineNumbers;
	}

	for (const Annotation& annotation : m_annotations)
	{
		if (annotation.isActive)
		{
			for (int i = annotation.startLine; i <= annotation.endLine; i++)
			{
				activeLineNumbers.insert(i);
			}
		}
	}

	if (activeLineNumbers.size())
	{
		for (const Annotation& annotation : m_annotations)
		{
			if (annotation.isFocused)
			{
				for (int i = annotation.startLine; i <= annotation.endLine; i++)
				{
					activeLineNumbers.insert(i);
				}
			}
		}
	}

	return activeLineNumbers;
}

std::vector<QRect> QtCodeArea::getCursorRectsForAnnotation(const Annotation& annotation) const
{
	std::vector<QRect> rects;

	QTextCursor cursor = textCursor();
	cursor.clearSelection();
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
				document()->findBlockByLineNumber(line - m_startLineNumber).length() != annotation.endCol)
			{
				cursor.setPosition(annotation.end);
			}
		}
		else
		{
			cursor.setPosition(toTextEditPosition(line, document()->findBlockByLineNumber(line - m_startLineNumber).length() - 1));
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
		std::vector<std::string> types = { "token", "local_symbol", "scope", "error", "fulltextmatch"};

		std::vector<std::string> states = { "normal", "focus", "active"};

		for (const std::string& type : types)
		{
			for (const std::string& state : states)
			{
				AnnotationColor color;
				color.border = scheme->getColor("code/snippet/selection/" + type + "/" + state + "/border");
				color.fill = scheme->getColor("code/snippet/selection/" + type + "/" + state + "/fill");
				color.text = scheme->getColor("code/snippet/selection/" + type + "/" + state + "/text");
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
	else if (annotation.isError)
	{
		i = 9;
	}
	else if (annotation.locationType == LOCATION_FULLTEXTSEARCH_MATCH)
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
