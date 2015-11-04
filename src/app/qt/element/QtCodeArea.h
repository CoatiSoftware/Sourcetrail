#ifndef QT_CODE_AREA_H
#define QT_CODE_AREA_H

#include <vector>
#include <memory>

#include <QPlainTextEdit>

#include "utility/types.h"

class QDragMoveEvent;
class QPaintEvent;
class QResizeEvent;
class QSize;
class QtCodeFile;
class QtCodeSnippet;
class QtHighlighter;
class QWidget;
class TokenLocation;
class TokenLocationFile;

class MouseWheelOverScrollbarFilter
	: public QObject
{
	Q_OBJECT

public:
	MouseWheelOverScrollbarFilter(QObject* parent);

protected:
	bool eventFilter(QObject*obj, QEvent* event);
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
		LineNumberArea(QtCodeArea *codeArea);
		virtual ~LineNumberArea();

		QSize sizeHint() const;

	protected:
		virtual void paintEvent(QPaintEvent* event);

	private:
		QtCodeArea *m_codeArea;
	};

	QtCodeArea(
		uint startLineNumber,
		const std::string& code,
		std::shared_ptr<TokenLocationFile> locationFile,
		QtCodeFile* file,
		QtCodeSnippet* parent
	);
	virtual ~QtCodeArea();

	virtual QSize sizeHint() const;

	uint getStartLineNumber() const;
	uint getEndLineNumber() const;

	std::shared_ptr<TokenLocationFile> getTokenLocationFile() const;

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberDigits() const;
	int lineNumberAreaWidth() const;
	void updateLineNumberAreaWidthForDigits(int digits);

	void updateContent();

	bool isActive() const;

protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent* event);
	virtual void paintEvent(QPaintEvent* event);
	virtual void enterEvent(QEvent* event);
	virtual void leaveEvent(QEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);

	virtual void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect &, int);
	void clearSelection();
	void setIDECursorPosition();

private:
	struct Annotation
	{
		int start;
		int end;
		Id tokenId;
		Id locationId;
		bool isScope;
	};

	struct ScopeAnnotation
	{
		ScopeAnnotation();
		bool operator!=(const ScopeAnnotation& other) const;

		int startLine;
		int endLine;
		Id tokenId;
		bool isFocused;
	};

	const Annotation* findAnnotationForPosition(int pos) const;
	std::vector<Id> findLocationIdsForPosition(int pos) const;

	void createAnnotations(std::shared_ptr<TokenLocationFile> locationFile);
	void annotateText();

	void setHoveredAnnotation(const Annotation* annotation);

	bool locationBelongsToSnippet(TokenLocation* location) const;

	int toTextEditPosition(int lineNumber, int columnNumber) const;
	std::pair<int, int> toLineColumn(int textEditPosition) const;
	int startTextEditPosition() const;
	int endTextEditPosition() const;

	void createActions();

	QtCodeFile* m_fileWidget;

	QWidget* m_lineNumberArea;
	QtHighlighter* m_highlighter;

	const uint m_startLineNumber;

	std::shared_ptr<TokenLocationFile> m_locationFile;

	std::vector<Annotation> m_annotations;
	std::vector<ScopeAnnotation> m_scopeAnnotations;

	const Annotation* m_hoveredAnnotation;
	std::vector<Id> m_hoveredLocationIds;

	int m_digits;
	int m_panningValue; // just for horizontal panning

	QAction* m_setIDECursorPositionAction;
	QPoint m_eventPosition; // is needed for IDE cursor control via context menu
							// the position where the context menu is opened needs to be stored
};

#endif // QT_CODE_AREA_H
