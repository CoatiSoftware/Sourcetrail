#ifndef QT_CODE_SNIPPET_H
#define QT_CODE_SNIPPET_H

#include <vector>

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

class QtCodeSnippet: public QPlainTextEdit
{
	Q_OBJECT

public:
	class LineNumberArea: public QWidget
	{
	public:
		LineNumberArea(QtCodeSnippet *codeSnippet);
		virtual ~LineNumberArea();

		QSize sizeHint() const;

	protected:
		void paintEvent(QPaintEvent* event);

	private:
		QtCodeSnippet *m_codeSnippet;
	};

	QtCodeSnippet(
		uint startLineNumber,
		const std::string& code,
		const TokenLocationFile& locationFile,
		QtCodeFile* parent
	);
	virtual ~QtCodeSnippet();

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
	void createAnnotations(const TokenLocationFile& locationFile);
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

#endif // QT_CODE_SNIPPET_H
