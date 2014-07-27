#ifndef QT_CODE_SNIPPET_H
#define QT_CODE_SNIPPET_H

#include <vector>

#include <QPlainTextEdit>

#include "utility/types.h"

class QPaintEvent;
class QPushButton;
class QResizeEvent;
class QSize;
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
		void paintEvent(QPaintEvent* event);

	private:
		QtCodeSnippet *m_codeSnippet;
	};

	QtCodeSnippet(
		int startLineNumber,
		const std::string& code,
		const TokenLocationFile& locationFile,
		const std::vector<Id>& activeTokenIds,
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
	virtual void enterEvent(QEvent* event);
	virtual void leaveEvent(QEvent* event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect &, int);
	void clickedTokenLocation();
	void clickedMaximizeButton();
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

	QtHighlighter* m_highlighter;

	QWidget* m_lineNumberArea;
	QPushButton* m_maximizeButton;

	const int m_startLineNumber;
	const std::vector<Id> m_activeTokenIds;
	std::vector<Annotation> m_annotations;
	int m_digits;

	const std::string m_filePath;
};

#endif // QT_CODE_SNIPPET_H
