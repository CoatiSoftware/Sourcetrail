#ifndef QT_CODE_AREA_H
#define QT_CODE_AREA_H

#include <memory>
#include <set>
#include <vector>

#include <QPlainTextEdit>

#include "data/location/LocationType.h"
#include "qt/utility/QtScrollSpeedChangeListener.h"
#include "utility/types.h"

class QDragMoveEvent;
class QPaintEvent;
class QResizeEvent;
class QSize;
class QtCodeNavigator;
class QtHighlighter;
class QWidget;
class SourceLocation;
class SourceLocationFile;


class MouseWheelOverScrollbarFilter
	: public QObject
{
	Q_OBJECT

public:
	MouseWheelOverScrollbarFilter();

protected:
	bool eventFilter(QObject* obj, QEvent* event);
};


class QtCodeArea
	: public QPlainTextEdit
{
	Q_OBJECT

public:
	class LineNumberArea
		: public QWidget
	{
	public:
		LineNumberArea(QtCodeArea* codeArea);
		virtual ~LineNumberArea();

		QSize sizeHint() const Q_DECL_OVERRIDE;

	protected:
		virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

	private:
		QtCodeArea* m_codeArea;
	};

	static void clearAnnotationColors();

	QtCodeArea(
		uint startLineNumber,
		const std::string& code,
		std::shared_ptr<SourceLocationFile> locationFile,
		QtCodeNavigator* navigator,
		QWidget* parent = nullptr
	);
	virtual ~QtCodeArea();

	virtual QSize sizeHint() const Q_DECL_OVERRIDE;

	uint getStartLineNumber() const;
	uint getEndLineNumber() const;

	std::shared_ptr<SourceLocationFile> getSourceLocationFile() const;

	void lineNumberAreaPaintEvent(QPaintEvent* event);
	int lineNumberDigits() const;
	int lineNumberAreaWidth() const;
	void updateLineNumberAreaWidthForDigits(int digits);

	void updateContent();

	void setIsActiveFile(bool isActiveFile);

	uint getLineNumberForLocationId(Id locationId) const;
	uint getStartLineNumberOfFirstActiveLocationOfTokenId(Id tokenId) const;
	Id getLocationIdOfFirstActiveLocationOfTokenId(Id tokenId) const;
	uint getActiveLocationCount() const;

	QRectF getLineRectForLineNumber(uint lineNumber) const;

	std::string getCode() const;

	void hideLineNumbers();

protected:
	virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
	virtual void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;
	virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
	virtual void enterEvent(QEvent* event) Q_DECL_OVERRIDE;
	virtual void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

	virtual void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;

private slots:
	void updateLineNumberAreaWidth(int newBlockCount = 0);
	void updateLineNumberArea(const QRect&, int);
	void clearSelection();
	void setNewTextCursor(const QTextCursor& cursor);
	void setIDECursorPosition();

private:
	struct Annotation
	{
		int startLine;
		int endLine;

		int startCol;
		int endCol;

		int start;
		int end;

		std::set<Id> tokenIds;
		Id locationId;

		LocationType locationType;

		bool isActive;
		bool isFocused;

		QColor oldTextColor;
	};

	struct AnnotationColor
	{
		std::string border;
		std::string fill;
		std::string text;
	};

	std::vector<const Annotation*> getInteractiveAnnotationsForPosition(int pos) const;
	void activateSourceLocations(const std::vector<const Annotation*>& annotations);
	void activateLocalSymbols(const std::vector<const Annotation*>& annotations);
	void activateErrors(const std::vector<const Annotation*>& annotations);

	void createAnnotations(std::shared_ptr<SourceLocationFile> locationFile);
	void annotateText();

	void setHoveredAnnotations(const std::vector<const Annotation*>& annotations);

	int toTextEditPosition(int lineNumber, int columnNumber) const;
	std::pair<int, int> toLineColumn(int textEditPosition) const;
	int startTextEditPosition() const;
	int endTextEditPosition() const;

	std::set<int> getActiveLineNumbers() const;

	std::vector<QRect> getCursorRectsForAnnotation(const Annotation& annotation) const;
	const AnnotationColor& getAnnotationColorForAnnotation(const Annotation& annotation);
	void setTextColorForAnnotation(Annotation& annotation, QColor color) const;

	void createActions();

	void createLineLengthCache();

	static std::vector<AnnotationColor> s_annotationColors;

	QtCodeNavigator* m_navigator;

	QWidget* m_lineNumberArea;
	QtHighlighter* m_highlighter;

	const uint m_startLineNumber;
	const std::string m_code;

	std::shared_ptr<SourceLocationFile> m_locationFile;

	std::vector<Annotation> m_annotations;
	std::vector<const Annotation*> m_hoveredAnnotations;

	// Remove when annotations become unique regarding start and end, and store multiple tokenIds
	std::vector<Annotation*> m_colorChangedAnnotations;

	int m_digits;

	bool m_isSelecting;
	bool m_isPanning;
	QPoint m_oldMousePosition;
	int m_panningDistance;

	QAction* m_setIDECursorPositionAction;
	QPoint m_eventPosition; // is needed for IDE cursor control via context menu
							// the position where the context menu is opened needs to be stored]

	bool m_isActiveFile;
	bool m_lineNumbersHidden;
	bool m_wasAnnotated;

	std::vector<int> m_lineLengths;
	int m_endTextEditPosition;

	QtScrollSpeedChangeListener m_scrollSpeedChangeListener;
};

#endif // QT_CODE_AREA_H
