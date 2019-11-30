#include "QtCodeArea.h"

#include <algorithm>

#include <QApplication>
#include <QDrag>
#include <QFont>
#include <QHBoxLayout>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QToolTip>

#include "ApplicationSettings.h"
#include "ColorScheme.h"
#include "MessageActivateLocalSymbols.h"
#include "MessageFocusIn.h"
#include "MessageFocusOut.h"
#include "MessageMoveIDECursor.h"
#include "MessageShowError.h"
#include "QtCodeNavigator.h"
#include "QtContextMenu.h"
#include "SourceLocationFile.h"
#include "TextCodec.h"
#include "utility.h"
#include "utilityApp.h"
#include "utilityString.h"

MouseWheelOverScrollbarFilter::MouseWheelOverScrollbarFilter() {}

bool MouseWheelOverScrollbarFilter::eventFilter(QObject* obj, QEvent* event)
{
	QScrollBar* scrollbar = dynamic_cast<QScrollBar*>(obj);
	if (event->type() == QEvent::Wheel && scrollbar)
	{
		QRect scrollbarArea(scrollbar->pos(), scrollbar->size());
		QPoint globalMousePos = dynamic_cast<QWheelEvent*>(event)->globalPos();
		QPoint localMousePos = scrollbar->mapFromGlobal(globalMousePos);

		// instead of "scrollbar->underMouse()" we need this check implemented here because
		// "underMouse()" does not work when the mouse enters the area without being moved
		if (scrollbarArea.contains(localMousePos))
		{
			event->ignore();
			return true;
		}
	}
	return QObject::eventFilter(obj, event);
}

QtLineNumberArea::QtLineNumberArea(QtCodeArea* codeArea): QWidget(codeArea), m_codeArea(codeArea)
{
	setObjectName("line_number_area");
}

QtLineNumberArea::~QtLineNumberArea() {}

QSize QtLineNumberArea::sizeHint() const
{
	return QSize(m_codeArea->lineNumberAreaWidth(), 0);
}

void QtLineNumberArea::paintEvent(QPaintEvent* event)
{
	m_codeArea->lineNumberAreaPaintEvent(event);
}


QtCodeArea::QtCodeArea(
	size_t startLineNumber,
	const std::string& code,
	std::shared_ptr<SourceLocationFile> locationFile,
	QtCodeNavigator* navigator,
	bool showLineNumbers,
	QWidget* parent)
	: QtCodeField(startLineNumber, code, locationFile, parent)
	, m_navigator(navigator)
	, m_digits(0)
	, m_isSelecting(false)
	, m_isPanning(false)
	, m_isDragging(false)
	, m_setIDECursorPositionAction(nullptr)
	, m_eventPosition(0, 0)
	, m_isActiveFile(false)
	, m_showLineNumbers(showLineNumbers)
{
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);

	m_lineNumberArea = new QtLineNumberArea(this);

	m_digits = lineNumberDigits();
	updateLineNumberAreaWidth();

	connect(this, &QtCodeArea::blockCountChanged, this, &QtCodeArea::updateLineNumberAreaWidth);
	connect(this, &QtCodeArea::updateRequest, this, &QtCodeArea::updateLineNumberArea);
	connect(this, &QtCodeArea::copyAvailable, this, &QtCodeArea::setCopyAvailable);

	// MouseWheelOverScrollbarFilter is deleted by parent.
	horizontalScrollBar()->installEventFilter(new MouseWheelOverScrollbarFilter());
	m_scrollSpeedChangeListener.setScrollBar(horizontalScrollBar());

	createActions();
}

QtCodeArea::~QtCodeArea()
{
	if (m_setIDECursorPositionAction != nullptr)
	{
		m_setIDECursorPositionAction->disconnect();
		m_setIDECursorPositionAction->deleteLater();
	}
}

QSize QtCodeArea::sizeHint() const
{
	double height = 0;
	double width = 0;

	for (QTextBlock block = document()->firstBlock(); block.isValid(); block = block.next())
	{
		QRectF rect = blockBoundingGeometry(block);
		height += rect.height();
		width = std::max(rect.width(), width);
	}

	if (horizontalScrollBar()->minimum() != horizontalScrollBar()->maximum() &&
		utility::getOsType() != OS_MAC)
	{
		height += horizontalScrollBar()->height();
	}

	return QSize(width + lineNumberAreaWidth() + 1, height + 5);
}

void QtCodeArea::lineNumberAreaPaintEvent(QPaintEvent* event)
{
	QPainter painter(m_lineNumberArea);

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = static_cast<int>(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom = top + static_cast<int>(blockBoundingRect(block).height());

	std::set<int> activeLineNumbers;
	std::set<int> focusedLineNumbers;

	const std::set<Id>& activeSymbolIds = m_navigator->getActiveTokenIds();
	const std::set<Id>& activeLocalTokenIds = m_navigator->getActiveLocalTokenIds();

	for (const Annotation& annotation: m_annotations)
	{
		bool focus = false;
		bool active = false;

		switch (annotation.locationType)
		{
		case LOCATION_LOCAL_SYMBOL:
			if (annotation.isActive || annotation.isFocused)
			{
				focus = true;
			}
			break;

		case LOCATION_ERROR:
		case LOCATION_FULLTEXT_SEARCH:
		case LOCATION_SCREEN_SEARCH:
			if (annotation.isFocused || annotation.isActive)
			{
				focus = true;
			}
			else
			{
				active = true;
			}
			break;

		case LOCATION_TOKEN:
		case LOCATION_SCOPE:
			if (annotation.isActive && activeLocalTokenIds.size())
			{
				focus = true;
				break;
			}
			else if (annotation.isFocused && utility::shareElement(activeSymbolIds, annotation.tokenIds))
			{
				active = true;
				break;
			}

		default:
			if (annotation.isActive)
			{
				active = true;
			}
			else if (annotation.isFocused)
			{
				focus = true;
			}
			break;
		}

		if (active || focus)
		{
			for (int i = annotation.startLine; i <= annotation.endLine; i++)
			{
				if (active)
				{
					activeLineNumbers.insert(i);
				}
				else
				{
					focusedLineNumbers.insert(i);
				}
			}
		}
	}

	ColorScheme* scheme = ColorScheme::getInstance().get();

	QColor textColor(scheme->getColor("code/snippet/line_number/text").c_str());
	QColor inactiveTextColor(scheme->getColor("code/snippet/line_number/inactive_text").c_str());
	QColor activeMarkerColor(scheme->getColor("code/snippet/line_number/marker/active").c_str());
	QColor focusedMarkerColor(scheme->getColor("code/snippet/line_number/marker/focus").c_str());

	QPen p = painter.pen();

	int drawAreaTop = event->rect().top();
	int drawAreaBottom = event->rect().bottom() + 1;

	if (horizontalScrollBar()->minimum() != horizontalScrollBar()->maximum() &&
		utility::getOsType() != OS_MAC && drawAreaBottom > height() - horizontalScrollBar()->height())
	{
		drawAreaBottom = height() - horizontalScrollBar()->height();
	}

	while (block.isValid() && top <= drawAreaBottom)
	{
		if (block.isVisible() && bottom >= drawAreaTop)
		{
			const int number = blockNumber + getStartLineNumber();
			const int height = bottom - top - std::max(0, bottom - drawAreaBottom);

			p.setColor(textColor);

			if (focusedLineNumbers.find(number) != focusedLineNumbers.end())
			{
				painter.fillRect(m_lineNumberArea->width() - 8, top, 3, height, focusedMarkerColor);
			}
			else if (activeLineNumbers.find(number) != activeLineNumbers.end())
			{
				painter.fillRect(m_lineNumberArea->width() - 8, top, 3, height, activeMarkerColor);
			}
			else if (!m_isActiveFile)
			{
				p.setColor(inactiveTextColor);
			}

			painter.setPen(p);
			painter.drawText(
				0, top, m_lineNumberArea->width() - 16, height, Qt::AlignRight, QString::number(number));
		}

		block = block.next();
		top = bottom;
		bottom = top + static_cast<int>(blockBoundingRect(block).height());
		blockNumber++;
	}
}

int QtCodeArea::lineNumberDigits() const
{
	return utility::digits(getEndLineNumber());
}

int QtCodeArea::lineNumberAreaWidth() const
{
	if (m_showLineNumbers)
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

void QtCodeArea::updateSourceLocations(std::shared_ptr<SourceLocationFile> locationFile)
{
	if (locationFile->getSourceLocationCount() > getSourceLocationFile()->getSourceLocationCount())
	{
		if (m_hoveredAnnotations.size())
		{
			setHoveredAnnotations({});
		}

		createAnnotations(locationFile);

		annotateText();
	}
}

void QtCodeArea::updateContent()
{
	annotateText();
}

void QtCodeArea::setIsActiveFile(bool isActiveFile)
{
	m_isActiveFile = isActiveFile;
}

size_t QtCodeArea::getLineNumberForLocationId(Id locationId) const
{
	for (const Annotation& annotation: m_annotations)
	{
		if (annotation.locationId == locationId)
		{
			return annotation.startLine;
		}
	}

	return 0;
}

std::pair<size_t, size_t> QtCodeArea::getLineNumbersForLocationId(Id locationId) const
{
	for (const Annotation& annotation: m_annotations)
	{
		if (annotation.locationId == locationId)
		{
			return std::pair<size_t, size_t>(annotation.startLine, annotation.endLine);
		}
	}

	return std::pair<size_t, size_t>(0, 0);
}

Id QtCodeArea::getLocationIdOfFirstActiveLocation(Id tokenId) const
{
	for (const Annotation& annotation: m_annotations)
	{
		if (annotation.locationType == LocationType::LOCATION_TOKEN && annotation.isActive &&
			(!tokenId || annotation.tokenIds.find(tokenId) != annotation.tokenIds.end()))
		{
			return annotation.locationId;
		}
	}

	return 0;
}

Id QtCodeArea::getLocationIdOfFirstActiveScopeLocation(Id tokenId) const
{
	for (const Annotation& annotation: m_annotations)
	{
		if (annotation.locationType == LocationType::LOCATION_SCOPE && annotation.isActive &&
			annotation.tokenIds.find(tokenId) != annotation.tokenIds.end())
		{
			return annotation.locationId;
		}
	}

	return 0;
}

Id QtCodeArea::getLocationIdOfFirstHighlightedLocation() const
{
	for (const Annotation& annotation: m_annotations)
	{
		if ((annotation.locationType == LocationType::LOCATION_TOKEN && annotation.isActive) ||
			annotation.locationType == LocationType::LOCATION_FULLTEXT_SEARCH ||
			annotation.locationType == LocationType::LOCATION_ERROR)
		{
			return annotation.locationId;
		}
	}

	return 0;
}

size_t QtCodeArea::getActiveLocationCount() const
{
	size_t count = 0;

	for (const Annotation& annotation: m_annotations)
	{
		if (annotation.locationType == LocationType::LOCATION_TOKEN &&
			(annotation.isActive || annotation.isFocused))
		{
			count++;
		}
	}

	if (!count)
	{
		for (const Annotation& annotation: m_annotations)
		{
			if (annotation.locationType == LocationType::LOCATION_FULLTEXT_SEARCH ||
				annotation.locationType == LocationType::LOCATION_ERROR)
			{
				count++;
			}
		}
	}

	return count;
}

QRectF QtCodeArea::getLineRectForLineNumber(size_t lineNumber) const
{
	if (lineNumber < getStartLineNumber())
	{
		lineNumber = getStartLineNumber();
	}
	else if (lineNumber > getEndLineNumber())
	{
		lineNumber = getEndLineNumber();
	}

	QTextBlock block = document()->findBlockByLineNumber(lineNumber - getStartLineNumber());
	return blockBoundingGeometry(block);
}

void QtCodeArea::findScreenMatches(
	const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches)
{
	TextCodec codec(ApplicationSettings::getInstance()->getTextEncoding());
	// remove carriage return
	const std::wstring& code = utility::toLowerCase(
		codec.decode(utility::replace(getCode(), "\r", "")));
	size_t pos = 0;
	while (pos != std::string::npos)
	{
		pos = code.find(query, pos);
		if (pos == std::string::npos)
		{
			break;
		}

		Annotation matchAnnotation;
		matchAnnotation.start = pos;
		matchAnnotation.end = pos + query.size();

		std::pair<int, int> start = toLineColumn(matchAnnotation.start);
		matchAnnotation.startLine = start.first;
		matchAnnotation.startCol = start.second;

		std::pair<int, int> end = toLineColumn(matchAnnotation.end);
		matchAnnotation.endLine = end.first;
		matchAnnotation.endCol = end.second;

		// Set first 2 bits to 1 to avoid collisions
		matchAnnotation.locationId = ~(~Id(0) >> 2) + screenMatches->size() + 1;
		matchAnnotation.locationType = LOCATION_SCREEN_SEARCH;

		matchAnnotation.isActive = false;
		matchAnnotation.isFocused = false;

		m_annotations.push_back(matchAnnotation);
		screenMatches->push_back(std::make_pair(this, matchAnnotation.locationId));

		pos += query.size();
	}

	if (screenMatches->size() && screenMatches->back().first == this)
	{
		viewport()->update();
	}
}

void QtCodeArea::clearScreenMatches()
{
	size_t i = m_annotations.size();
	while (i > 0 && m_annotations[i - 1].locationType == LOCATION_SCREEN_SEARCH)
	{
		i--;
		m_linesToRehighlight.push_back(m_annotations[i].startLine - getStartLineNumber());
	}

	if (i != m_annotations.size())
	{
		m_annotations.erase(m_annotations.begin() + i, m_annotations.end());
		viewport()->update();
	}
}

void QtCodeArea::ensureLocationIdVisible(Id locationId, int parentWidth, bool animated)
{
	QScrollBar* scrollBar = horizontalScrollBar();
	if (!scrollBar || scrollBar->minimum() == scrollBar->maximum())
	{
		return;
	}

	const int oldValue = scrollBar->value();
	scrollBar->setValue(scrollBar->minimum());

	const Annotation* annotationPtr = nullptr;
	for (const Annotation& annotation: m_annotations)
	{
		if (annotation.locationId == locationId)
		{
			annotationPtr = &annotation;
			break;
		}
	}

	if (!annotationPtr)
	{
		return;
	}

	std::vector<QRect> rects = getCursorRectsForAnnotation(*annotationPtr);
	QRect boundingRect;
	for (QRect rect: rects)
	{
		if (boundingRect.width())
		{
			boundingRect = boundingRect.united(rect);
		}
		else
		{
			boundingRect = rect;
		}
	}

	if (!boundingRect.width())
	{
		return;
	}

	boundingRect.adjust(-50, 0, 50, 0);

	int totalWidth = sizeHint().width() - lineNumberAreaWidth();
	int visibleWidth = parentWidth - lineNumberAreaWidth();

	int targetWidth = 0;
	if (boundingRect.right() > visibleWidth)
	{
		targetWidth = boundingRect.right() - visibleWidth;

		if (targetWidth > boundingRect.left())
		{
			targetWidth = boundingRect.left();
		}
	}

	const double percentTarget = double(targetWidth) / (totalWidth - visibleWidth);
	const int newValue = (scrollBar->maximum() - scrollBar->minimum()) * percentTarget +
		scrollBar->minimum();

	if (animated && ApplicationSettings::getInstance()->getUseAnimations())
	{
		QPropertyAnimation* anim = new QPropertyAnimation(scrollBar, "value");
		anim->setDuration(300);
		anim->setStartValue(oldValue);
		anim->setEndValue(newValue);
		anim->setEasingCurve(QEasingCurve::InOutQuad);
		anim->start();
	}
	else
	{
		scrollBar->setValue(newValue);
	}
}

void QtCodeArea::resizeEvent(QResizeEvent* e)
{
	QPlainTextEdit::resizeEvent(e);

	QRect cr = contentsRect();
	m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void QtCodeArea::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		const QPoint clickPosition(event->pos());
		m_oldMousePosition = clickPosition;
		if (isSelectionPosition(clickPosition))
		{
			m_isDragging = true;
		}
		else
		{
			clearSelection();

			m_panningDistance = 0;

			if (Qt::KeyboardModifier::ShiftModifier & QApplication::keyboardModifiers())
			{
				m_isPanning = true;
				viewport()->setCursor(Qt::ClosedHandCursor);
			}
			else
			{
				m_isSelecting = true;
				QTextCursor cursor = this->cursorForPosition(event->pos());
				setNewTextCursor(cursor);

				viewport()->setCursor(Qt::IBeamCursor);
			}
		}
	}
}

void QtCodeArea::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
	{
		QtCodeField::mouseReleaseEvent(event);
		return;
	}

	m_isSelecting = false;
	m_isPanning = false;

	if (m_isDragging)
	{
		const int distance = (event->pos() - m_oldMousePosition).manhattanLength();
		if (distance < QApplication::startDragDistance())
		{
			clearSelection();
		}

		m_isDragging = false;
		return;
	}

	viewport()->setCursor(Qt::ArrowCursor);

	const int panningThreshold = 5;
	if (m_panningDistance < panningThreshold)	 // dont do anything if mouse is release to end
												 // some real panning action.
	{
		if (Qt::KeyboardModifier::ControlModifier & QApplication::keyboardModifiers())
		{
			m_eventPosition = event->pos();
			setIDECursorPosition();
		}
		else
		{
			std::vector<const Annotation*> annotations = getInteractiveAnnotationsForPosition(
				event->pos());
			if (annotations.size())
			{
				activateAnnotationsOrErrors(annotations);
			}
			else if (m_navigator->getActiveLocalTokenIds().size())
			{
				MessageActivateLocalSymbols(std::vector<Id>()).dispatch();
			}
		}
	}
}

void QtCodeArea::mouseMoveEvent(QMouseEvent* event)
{
	const QPoint currentMousePosition = event->pos();
	const int delta = (currentMousePosition - m_oldMousePosition).manhattanLength();
	const int deltaX = currentMousePosition.x() - m_oldMousePosition.x();
	const int deltaY = currentMousePosition.y() - m_oldMousePosition.y();
	m_oldMousePosition = currentMousePosition;
	m_panningDistance += abs(deltaX + deltaY);

	if (m_isSelecting)
	{
		QTextCursor cursor = textCursor();
		cursor.setPosition(this->cursorForPosition(event->pos()).position(), QTextCursor::KeepAnchor);
		setNewTextCursor(cursor);
	}
	else if (m_isPanning)
	{
		QScrollBar* scrollbar = horizontalScrollBar();
		int visibleContentWidth = width() - lineNumberAreaWidth();
		float deltaPosRatio = float(deltaX) / (visibleContentWidth);
		scrollbar->setValue(scrollbar->value() - std::round(deltaPosRatio * scrollbar->pageStep()));
	}
	else if (m_isDragging)
	{
		if (delta >= QApplication::startDragDistance())
		{
			dragSelectedText();
			m_isDragging = false;
		}
	}

	std::vector<const Annotation*> annotations = getInteractiveAnnotationsForPosition(event->pos());

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
		if (m_navigator->hasErrors())
		{
			for (const Annotation* annotation: annotations)
			{
				if (annotation->locationType == LOCATION_ERROR && annotation->tokenIds.size())
				{
					std::wstring errorMessage = m_navigator->getErrorMessageForId(
						*annotation->tokenIds.begin());
					QToolTip::showText(
						event->globalPos(), QString::fromStdWString(errorMessage), this);

					QtCodeField::focusTokenIds({*annotation->tokenIds.begin()});
					viewport()->setCursor(Qt::PointingHandCursor);
					return;
				}
			}
		}

		QToolTip::hideText();

		setHoveredAnnotations(annotations);
	}
}

void QtCodeArea::wheelEvent(QWheelEvent* event)
{
	if ((event->angleDelta().x() != 0 &&
		 horizontalScrollBar()->minimum() != horizontalScrollBar()->maximum()) ||
		(event->angleDelta().y() != 0 &&
		 verticalScrollBar()->minimum() != verticalScrollBar()->maximum()))
	{
		QPlainTextEdit::wheelEvent(event);
	}
	else
	{
		event->ignore();
	}
}

void QtCodeArea::contextMenuEvent(QContextMenuEvent* event)
{
	if (m_setIDECursorPositionAction != nullptr)
	{
		m_eventPosition = event->pos();

		checkOpenInTabActionEnabled(event->pos());
		m_setIDECursorPositionAction->setEnabled(!getSourceLocationFile()->getFilePath().empty());

		QtContextMenu menu(event, this);
		menu.addAction(m_openInTabAction);
		menu.addUndoActions();
		menu.addSeparator();
		menu.addFileActions(getSourceLocationFile()->getFilePath());
		menu.addSeparator();
		menu.addAction(m_copyAction);
		menu.addAction(m_setIDECursorPositionAction);
		menu.show();
	}
}

void QtCodeArea::focusTokenIds(const std::vector<Id>& tokenIds)
{
	MessageFocusIn(tokenIds, TOOLTIP_ORIGIN_CODE).dispatch();
}

void QtCodeArea::defocusTokenIds(const std::vector<Id>& tokenIds)
{
	MessageFocusOut(tokenIds).dispatch();
}

void QtCodeArea::updateLineNumberAreaWidth(int /* newBlockCount */)
{
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void QtCodeArea::updateLineNumberArea(const QRect& rect, int dy)
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

void QtCodeArea::setIDECursorPosition()
{
	std::pair<int, int> lineColumn = toLineColumn(this->cursorForPosition(m_eventPosition).position());

	MessageMoveIDECursor(getSourceLocationFile()->getFilePath(), lineColumn.first, lineColumn.second)
		.dispatch();
}

void QtCodeArea::setCopyAvailable(bool yes)
{
	m_copyAction->setEnabled(yes);
}

void QtCodeArea::clearSelection()
{
	QTextCursor cursor = textCursor();
	cursor.clearSelection();

	setNewTextCursor(cursor);
}

void QtCodeArea::setNewTextCursor(const QTextCursor& cursor)
{
	int horizontalValue = horizontalScrollBar()->value();
	int verticalValue = verticalScrollBar()->value();

	setTextCursor(cursor);

	horizontalScrollBar()->setValue(horizontalValue);
	verticalScrollBar()->setValue(verticalValue);
}

void QtCodeArea::dragSelectedText()
{
	QMimeData* mimeData = new QMimeData;
	const QString text = textCursor().selection().toPlainText();
	mimeData->setText(text);

	QDrag* drag = new QDrag(this);
	drag->setMimeData(mimeData);
	drag->exec(Qt::CopyAction);
}

bool QtCodeArea::isSelectionPosition(const QPoint positionPoint) const
{
	const QTextCursor textCursor = QtCodeArea::textCursor();
	const int selectionStart = textCursor.selectionStart();
	const int selectionEnd = textCursor.selectionEnd();
	const QTextCursor positionCursor = this->cursorForPosition(positionPoint);
	const int position = positionCursor.position();
	return selectionStart != selectionEnd && selectionStart <= position && position <= selectionEnd;
}

void QtCodeArea::activateAnnotationsOrErrors(const std::vector<const Annotation*>& annotations)
{
	if (m_navigator->hasErrors())
	{
		std::vector<Id> errorIds;
		for (const Annotation* annotation: annotations)
		{
			if (annotation->locationType == LOCATION_ERROR && annotation->tokenIds.size())
			{
				errorIds.insert(
					errorIds.end(), annotation->tokenIds.begin(), annotation->tokenIds.end());
			}
		}

		if (errorIds.size() == 1)
		{
			MessageShowError(errorIds[0]).dispatch();
			return;
		}
	}

	activateAnnotations(annotations);
}

void QtCodeArea::annotateText()
{
	const std::set<Id>& activeSymbolIds = m_navigator->getCurrentActiveTokenIds();
	const std::set<Id>& activeLocationIds = utility::concat(
		m_navigator->getCurrentActiveLocationIds(), m_navigator->getCurrentActiveLocalLocationIds());

	std::set<Id> focusedSymbolIds = m_navigator->getActiveTokenIds();
	utility::append(focusedSymbolIds, m_navigator->getActiveLocalTokenIds());

	for (Id currentActiveId: activeSymbolIds)
	{
		focusedSymbolIds.erase(currentActiveId);
	}
	utility::append(focusedSymbolIds, m_navigator->getFocusedTokenIds());

	bool needsUpdate = QtCodeField::annotateText(
		activeSymbolIds, activeLocationIds, focusedSymbolIds);
	if (needsUpdate)
	{
		m_lineNumberArea->update();
	}
}

void QtCodeArea::createActions()
{
	m_copyAction = new QAction(tr("Copy Selection"), this);
	m_copyAction->setStatusTip(tr("Copy selection to clipboard"));
	m_copyAction->setToolTip(tr("Copy selection to clipboard"));
	m_copyAction->setEnabled(false);
	connect(m_copyAction, &QAction::triggered, this, &QPlainTextEdit::copy);

	m_setIDECursorPositionAction = new QAction(tr("Show in IDE (Ctrl + Left Click)"), this);
#if defined(Q_OS_MAC)
	m_setIDECursorPositionAction->setText(tr("Show in IDE (Cmd + Left Click)"));
#endif
	m_setIDECursorPositionAction->setStatusTip(tr("Set the IDE Cursor to this code position"));
	m_setIDECursorPositionAction->setToolTip(tr("Set the IDE Cursor to this code position"));
	connect(
		m_setIDECursorPositionAction, &QAction::triggered, this, &QtCodeArea::setIDECursorPosition);
}
