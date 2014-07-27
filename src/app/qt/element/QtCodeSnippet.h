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

	void addMaximizeButton();

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberDigits() const;
	int lineNumberAreaWidth() const;
	void updateLineNumberAreaWidthForDigits(int digits);

	void setActiveTokenIds(const std::vector<Id>& activeTokenIds);

protected:
	virtual void resizeEvent(QResizeEvent *event);
	virtual void showEvent(QShowEvent* event);
	virtual void enterEvent(QEvent* event);
	virtual void leaveEvent(QEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

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

	void createAnnotations(const TokenLocationFile& locationFile);
	void annotateText();

	int toTextEditPosition(int lineNumber, int columnNumber) const;
	int startTextEditPosition() const;
	int endTextEditPosition() const;

	QWidget* m_lineNumberArea;
	QtHighlighter* m_highlighter;

	QPushButton* m_maximizeButton;
	bool m_showMaximizeButton;

	const int m_startLineNumber;
	const std::string m_filePath;
	std::vector<Id> m_activeTokenIds;

	std::vector<Annotation> m_annotations;
	int m_digits;
};

#endif // QT_CODE_SNIPPET_H
