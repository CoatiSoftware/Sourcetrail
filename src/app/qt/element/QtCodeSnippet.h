#ifndef QT_CODE_SNIPPET_H
#define QT_CODE_SNIPPET_H

#include <vector>

#include <QPlainTextEdit>
#include <QObject>

#include "utility/types.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QtCodeView;
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

	QtCodeSnippet(QtCodeView* parentView, int startLineNumber, QWidget *parent = 0);
	virtual ~QtCodeSnippet();

	void lineNumberAreaPaintEvent(QPaintEvent *event);
	int lineNumberAreaWidth();

	void annotateText(const TokenLocationFile& locationFile);

protected:
	void resizeEvent(QResizeEvent *event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect &, int);
	void clickTokenLocation();

private:
	struct Annotation
	{
		int start;
		int end;
		Id tokenId;
	};

	int toTextEditPosition(int lineNumber, int columnNumber) const;

	QWidget *m_lineNumberArea;
	QtCodeView* m_parentView;

	const int m_startLineNumber;
	std::vector<Annotation> m_annotations;
};

#endif // QT_CODE_SNIPPET_H
