#include "QtCodeField.h"

#include <QPainter>
#include <QTextBlock>
#include <QTextCodec>

#include "SourceLocation.h"
#include "SourceLocationFile.h"
#include "QtHighlighter.h"
#include "ApplicationSettings.h"
#include "ColorScheme.h"
#include "MessageActivateLocalSymbols.h"
#include "MessageActivateSourceLocations.h"
#include "MessageActivateTokenIds.h"
#include "MessageTooltipShow.h"
#include "TextCodec.h"
#include "tracing.h"
#include "utility.h"

std::vector<QtCodeField::AnnotationColor> QtCodeField::s_annotationColors;

void QtCodeField::clearAnnotationColors()
{
	s_annotationColors.clear();
}

QtCodeField::QtCodeField(
	uint startLineNumber,
	const std::string& code,
	std::shared_ptr<SourceLocationFile> locationFile,
	bool convertLocationsOnDemand,
	QWidget* parent
)
	: QPlainTextEdit(parent)
	, m_startLineNumber(startLineNumber)
	, m_code(code)
	, m_endTextEditPosition(0)
{
	TRACE();

	setObjectName("code_area");
	setReadOnly(true);
	setFrameStyle(QFrame::NoFrame);
	setLineWrapMode(QPlainTextEdit::NoWrap);
	setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	setMouseTracking(true);

	viewport()->setCursor(Qt::ArrowCursor);

	std::string displayCode = m_code;

	// avoid extra line at end of snippet
	if (!locationFile->isWhole())
	{
		if (!displayCode.empty() && *displayCode.rbegin() == '\n')
		{
			displayCode.pop_back();
		}

		if (!displayCode.empty() && *displayCode.rbegin() == '\r')
		{
			displayCode.pop_back();
		}
	}

	TextCodec codec(ApplicationSettings::getInstance()->getTextEncoding().c_str());
	if (convertLocationsOnDemand && codec.isValid())
	{
		QString convertedDisplayCode = QString::fromStdWString(codec.decode(displayCode));
		setPlainText(convertedDisplayCode);
		if (displayCode.size() != size_t(convertedDisplayCode.length()))
		{
			LOG_INFO("Converting displayed code to " + codec.getName() +
				" resulted in offset of source locations. Correcting this now.");
			createMultibyteCharacterLocationCache(convertedDisplayCode);
		}
	}
	else
	{
		setPlainText(displayCode.c_str());
	}

	createLineLengthCache();

	createAnnotations(locationFile);

	FilePath path = locationFile->getFilePath();
	LanguageType language = LANGUAGE_UNKNOWN;
	if (!path.empty())
	{
		language = (path.extension() == L".java" ? LANGUAGE_JAVA : LANGUAGE_CPP);
	}

	m_highlighter = std::make_shared<QtHighlighter>(document(), language);
	m_highlighter->highlightDocument();

	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	QFont font(appSettings->getFontName().c_str());
	font.setPixelSize(appSettings->getFontSize());
	setFont(font);
	setTabStopWidth(appSettings->getCodeTabWidth() * fontMetrics().width('9'));
}

QtCodeField::~QtCodeField()
{
}

QSize QtCodeField::sizeHint() const
{
	double height = 0;
	int width = 0;

	QFontMetrics fm = fontMetrics();
	for (QTextBlock block = document()->firstBlock(); block.isValid(); block = block.next())
	{
		QRectF rect = blockBoundingGeometry(block);
		height += rect.height();

		int blockWidth = fm.boundingRect(
			0, 0, 1000000, 1000000,
			Qt::AlignLeft | Qt::AlignTop | Qt::TextExpandTabs, block.text(), tabStopWidth()).width();

		width = std::max(blockWidth, width);
	}

	return QSize(width + 1, height + 5);
}

uint QtCodeField::getStartLineNumber() const
{
	return m_startLineNumber;
}

uint QtCodeField::getEndLineNumber() const
{
	return m_startLineNumber + blockCount() - 1;
}

int QtCodeField::totalLineHeight() const
{
	return blockBoundingRect(firstVisibleBlock()).height() * blockCount();
}

std::string QtCodeField::getCode() const
{
	return m_code;
}

std::shared_ptr<SourceLocationFile> QtCodeField::getSourceLocationFile() const
{
	return m_locationFile;
}

void QtCodeField::annotateText()
{
	annotateText(std::set<Id>(), std::set<Id>(), std::set<Id>());
}

void QtCodeField::paintEvent(QPaintEvent* event)
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
				firstVisibleLine = block.blockNumber();
			}
			lastVisibleLine = block.blockNumber();
		}

		block = block.next();
		top = bottom;
		bottom = top + static_cast<int>(blockBoundingRect(block).height());
	}

	m_highlighter->rehighlightLines(m_linesToRehighlight);
	m_linesToRehighlight.clear();

	// TODO: this causes another paint event if lines get rehighlighted
	m_highlighter->highlightRange(firstVisibleLine, lastVisibleLine);

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

		if (color.text != "transparent" &&
			QColor(color.text.c_str()) != m_highlighter->getFormat(annotation.start, annotation.end).foreground().color())
		{
			// TODO: this causes another paint event if text color changes
			setTextColorForAnnotation(annotation, QColor(color.text.c_str()));
		}

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

void QtCodeField::enterEvent(QEvent* event)
{
}

void QtCodeField::leaveEvent(QEvent* event)
{
	setHoveredAnnotations(std::vector<const Annotation*>());
}

void QtCodeField::mouseMoveEvent(QMouseEvent* event)
{
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
		setHoveredAnnotations(annotations);
	}
}

void QtCodeField::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() != Qt::LeftButton)
	{
		return;
	}

	viewport()->setCursor(Qt::ArrowCursor);

	QTextCursor cursor = this->cursorForPosition(event->pos());
	std::vector<const Annotation*> annotations = getInteractiveAnnotationsForPosition(cursor.position());

	if (!annotations.size())
	{
		return;
	}

	activateAnnotations(annotations);
}

void QtCodeField::focusTokenIds(const std::vector<Id>& focusedTokenIds)
{
	annotateText(std::set<Id>(), std::set<Id>(), std::set<Id>(focusedTokenIds.begin(), focusedTokenIds.end()));
}

void QtCodeField::defocusTokenIds(const std::vector<Id>& activeTokenIds)
{
	annotateText(std::set<Id>(activeTokenIds.begin(), activeTokenIds.end()), std::set<Id>(), std::set<Id>());
}

bool QtCodeField::annotateText(
	const std::set<Id>& activeSymbolIds, const std::set<Id>& activeLocationIds, const std::set<Id>& focusedSymbolIds)
{
	for (size_t i = 0; i < m_annotations.size(); i++)
	{
		Annotation& annotation = m_annotations[i];
		bool wasActive = annotation.isActive;
		bool wasFocused = annotation.isFocused;

		annotation.isActive = (
			utility::shareElement(activeSymbolIds, annotation.tokenIds) ||
			activeLocationIds.find(annotation.locationId) != activeLocationIds.end()
		);

		annotation.isFocused = utility::shareElement(focusedSymbolIds, annotation.tokenIds);

		if (annotation.locationType == LOCATION_QUALIFIER)
		{
			annotation.isActive = false;
		}

		if (wasFocused != annotation.isFocused || wasActive != annotation.isActive)
		{
			m_linesToRehighlight.push_back(annotation.startLine - m_startLineNumber);
		}
	}

	if (m_linesToRehighlight.size())
	{
		viewport()->update();
		return true;
	}

	return false;
}

void QtCodeField::createAnnotations(std::shared_ptr<SourceLocationFile> locationFile)
{
	TRACE();

	m_locationFile = locationFile;
	m_annotations.clear();

	uint endLineNumber = getEndLineNumber();
	std::set<Id> locationIds;

	locationFile->forEachSourceLocation(
		[&](const SourceLocation* location)
		{
			if (location->getType() == LOCATION_SIGNATURE || location->getType() == LOCATION_COMMENT)
			{
				return;
			}

			if (location->getLocationId() && locationIds.find(location->getLocationId()) != locationIds.end())
			{
				return;
			}
			locationIds.insert(location->getLocationId());

			Annotation annotation;

			const SourceLocation* startLocation = location->getStartLocation();
			if (!startLocation || startLocation->getLineNumber() < m_startLineNumber)
			{
				annotation.start = startTextEditPosition();
				annotation.startLine = m_startLineNumber;
				annotation.startCol = 0;
			}
			else if (startLocation->getLineNumber() <= endLineNumber)
			{
				const int startLine = startLocation->getLineNumber();
				const int startCol = getColumnCorrectedForMultibyteCharacters(startLine, startLocation->getColumnNumber() - 1);

				annotation.start = toTextEditPosition(startLine, startCol);
				annotation.startLine = startLine;
				annotation.startCol = startCol;
			}
			else
			{
				return;
			}

			const SourceLocation* endLocation = location->getEndLocation();
			if (!endLocation || endLocation->getLineNumber() > endLineNumber)
			{
				annotation.end = endTextEditPosition();
				annotation.endLine = endLineNumber;
				annotation.endCol = m_lineLengths[document()->blockCount() - 1];
			}
			else if (endLocation->getLineNumber() >= m_startLineNumber)
			{
				const int endLine = endLocation->getLineNumber();
				const int endCol = getColumnCorrectedForMultibyteCharacters(endLine, endLocation->getColumnNumber());

				annotation.end = toTextEditPosition(endLine, endCol);
				annotation.endLine = endLine;
				annotation.endCol = endCol;
			}
			else
			{
				return;
			}

			annotation.tokenIds.insert(location->getTokenIds().begin(), location->getTokenIds().end());
			annotation.locationId = location->getLocationId();
			annotation.locationType = location->getType();

			annotation.isActive = false;
			annotation.isFocused = false;

			m_annotations.push_back(annotation);
		}
	);
}

void QtCodeField::activateAnnotations(const std::vector<const Annotation*>& annotations)
{
	std::vector<Id> locationIds;
	std::set<Id> tokenIds;
	std::set<Id> localSymbolIds;

	for (const Annotation* annotation : annotations)
	{
		if (annotation->locationType == LOCATION_TOKEN || annotation->locationType == LOCATION_QUALIFIER)
		{
			if (annotation->locationId > 0)
			{
				locationIds.push_back(annotation->locationId);
			}

			if (annotation->tokenIds.size())
			{
				tokenIds.insert(annotation->tokenIds.begin(), annotation->tokenIds.end());
			}
		}
		else if (annotation->locationType == LOCATION_LOCAL_SYMBOL)
		{
			if (annotation->tokenIds.size())
			{
				localSymbolIds.insert(annotation->tokenIds.begin(), annotation->tokenIds.end());
			}
		}
	}

	if (tokenIds.size() > 1 || localSymbolIds.size() > 1 || (tokenIds.size() && localSymbolIds.size()))
	{
		MessageTooltipShow(locationIds, utility::toVector(localSymbolIds), TOOLTIP_ORIGIN_CODE).dispatch();
	}
	else if (locationIds.size())
	{
		MessageActivateSourceLocations(locationIds).dispatch();
	}
	else if (tokenIds.size()) // fallback for links in project description
	{
		MessageActivateTokenIds(utility::toVector(tokenIds)).dispatch();
	}
	else if (localSymbolIds.size())
	{
		MessageActivateLocalSymbols(utility::toVector(localSymbolIds)).dispatch();
	}
}

int QtCodeField::toTextEditPosition(int lineNumber, int columnNumber) const
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

std::pair<int, int> QtCodeField::toLineColumn(int textEditPosition) const
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

int QtCodeField::startTextEditPosition() const
{
	return 0;
}

int QtCodeField::endTextEditPosition() const
{
	return m_endTextEditPosition;
}

void QtCodeField::setHoveredAnnotations(const std::vector<const Annotation*>& annotations)
{
	if (m_hoveredAnnotations.size())
	{
		defocusTokenIds({});
	}

	m_hoveredAnnotations = annotations;

	if (annotations.size())
	{
		std::vector<Id> tokenIds;
		for (const Annotation* annotation : annotations)
		{
			tokenIds.insert(tokenIds.end(), annotation->tokenIds.begin(), annotation->tokenIds.end());
		}

		focusTokenIds(tokenIds);
	}
}

std::vector<QRect> QtCodeField::getCursorRectsForAnnotation(const Annotation& annotation) const
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

const QtCodeField::AnnotationColor& QtCodeField::getAnnotationColorForAnnotation(const Annotation& annotation)
{
	if (!s_annotationColors.size())
	{
		ColorScheme* scheme = ColorScheme::getInstance().get();
		std::vector<std::string> types = { "token", "local_symbol", "scope", "error", "fulltext_search", "screen_search" };
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
	else if (annotation.locationType == LOCATION_FULLTEXT_SEARCH)
	{
		i = 12;
	}
	else if (annotation.locationType == LOCATION_SCREEN_SEARCH)
	{
		i = 15;
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

void QtCodeField::setTextColorForAnnotation(const Annotation& annotation, QColor color) const
{
	QTextCharFormat format;
	format.setForeground(color);
	m_highlighter->applyFormat(annotation.start, annotation.end, format);
}

std::vector<const QtCodeField::Annotation*> QtCodeField::getInteractiveAnnotationsForPosition(int pos) const
{
	std::vector<const QtCodeField::Annotation*> annotations;

	for (const Annotation& annotation : m_annotations)
	{
		const LocationType& type = annotation.locationType;
		if ((type == LOCATION_TOKEN || type == LOCATION_QUALIFIER || type == LOCATION_LOCAL_SYMBOL || type == LOCATION_ERROR)
			&& pos >= annotation.start && pos <= annotation.end)
		{
			annotations.push_back(&annotation);
		}
	}

	return annotations;
}

void QtCodeField::createLineLengthCache()
{
	m_endTextEditPosition = -1;

	m_lineLengths.clear();

	for (QTextBlock it = document()->begin(); it != document()->end(); it = it.next())
	{
		m_lineLengths.push_back(it.length());
		m_endTextEditPosition += it.length();
	}
}

void QtCodeField::createMultibyteCharacterLocationCache(const QString& code)
{
	m_multibyteCharacterLocations.clear();
	QTextCodec* codec = QTextCodec::codecForName(ApplicationSettings::getInstance()->getTextEncoding().c_str());

	for (const QString& line: code.split("\n"))
	{
		std::vector<std::pair<int, int>> columnsToOffsets;
		for (int i = 0; i < line.size(); i++)
		{
			if (line[i].unicode() > 127)
			{
				int ss = codec->fromUnicode(line[i]).size();
				columnsToOffsets.push_back(std::make_pair(i, ss));
			}
		}
		m_multibyteCharacterLocations.push_back(columnsToOffsets);
	}
}

int QtCodeField::getColumnCorrectedForMultibyteCharacters(int line, int column) const
{
	if (line < int(m_startLineNumber))
	{
		return column;
	}

	const size_t relativeLineNumber = line - m_startLineNumber;
	if (relativeLineNumber < m_multibyteCharacterLocations.size())
	{
		for (const std::pair<int, int> m_multibyteCharacterLocation : m_multibyteCharacterLocations[relativeLineNumber])
		{
			if (column > m_multibyteCharacterLocation.first)
			{
				column -= m_multibyteCharacterLocation.second - 1;
			}
		}
	}
	return column;
}
