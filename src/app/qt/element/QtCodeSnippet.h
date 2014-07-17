#ifndef QT_CODE_SNIPPET_H
#define QT_CODE_SNIPPET_H

#include <vector>

#include <QPlainTextEdit>

#include "utility/types.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QtCodeView;
class QtHighlighter;
class QWidget;
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
		void paintEvent(QPaintEvent *event);

	private:
		QtCodeSnippet *m_codeSnippet;
	};

	QtCodeSnippet(
		QtCodeView* parentView,
		const std::string& code,
		const TokenLocationFile& locationFile,
		int startLineNumber,
		Id activeTokenId,
		QWidget *parent = 0
	);
	virtual ~QtCodeSnippet();

	QSize sizeHint() const;

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberDigits() const;
	int lineNumberAreaWidth() const;
	void updateLineNumberAreaWidthForDigits(int digits);

	void annotateText(const TokenLocationFile& locationFile);

protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent* event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect &, int);
	void clickTokenLocation();
	void clearSelection();

private:
	struct Annotation
	{
		int start;
		int end;
		Id tokenId;
	};

	int toTextEditPosition(int lineNumber, int columnNumber) const;
	int startTextEditPosition() const;
	int endTextEditPosition() const;

	QtCodeView* m_parentView;
	QtHighlighter* m_highlighter;

	QWidget *m_lineNumberArea;
	const int m_startLineNumber;
	const Id m_activeTokenId;
	int m_digits;

	std::vector<Annotation> m_annotations;
};

#endif // QT_CODE_SNIPPET_H
