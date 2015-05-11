#ifndef QT_CODE_AREA_H
#define QT_CODE_AREA_H

#include <vector>
#include <memory>

#include <QPlainTextEdit>

#include "utility/types.h"

class QPaintEvent;
class QPushButton;
class QResizeEvent;
class QSize;
class QtCodeFile;
class QtHighlighter;
class QWidget;
class TokenLocation;
class TokenLocationFile;

class QtCodeArea: public QPlainTextEdit
{
	Q_OBJECT

public:
	class LineNumberArea: public QWidget
	{
	public:
		LineNumberArea(QtCodeArea *codeArea);
		virtual ~LineNumberArea();

		QSize sizeHint() const;

	protected:
		void paintEvent(QPaintEvent* event);

	private:
		QtCodeArea *m_codeArea;
	};

	QtCodeArea(
		uint startLineNumber,
		const std::string& code,
		std::shared_ptr<TokenLocationFile> locationFile,
		QtCodeFile* parent
	);
	virtual ~QtCodeArea();

	QSize sizeHint() const;

	void addMaximizeButton();

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberDigits() const;
	int lineNumberAreaWidth() const;
	void updateLineNumberAreaWidthForDigits(int digits);

	void update();

protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent* event);
	virtual void enterEvent(QEvent* event);
	virtual void leaveEvent(QEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect &, int);
	void clickedMaximizeButton();
	void clearSelection();

private:
	struct Annotation
	{
		int start;
		int end;
		Id tokenId;
		Id locationId;
		bool isScope;
	};

	const Annotation* findAnnotationForPosition(int pos) const;
	void createAnnotations(std::shared_ptr<TokenLocationFile> locationFile);
	void annotateText();

	bool locationBelongsToSnippet(TokenLocation* location) const;

	int toTextEditPosition(int lineNumber, int columnNumber) const;
	int startTextEditPosition() const;
	int endTextEditPosition() const;

	QtCodeFile* m_parent;

	QWidget* m_lineNumberArea;
	QtHighlighter* m_highlighter;

	QPushButton* m_maximizeButton;

	const uint m_startLineNumber;

	std::vector<Annotation> m_annotations;
	const Annotation* m_hoveredAnnotation;

	int m_digits;
};

#endif // QT_CODE_AREA_H
