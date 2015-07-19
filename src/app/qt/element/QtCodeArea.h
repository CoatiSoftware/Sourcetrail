#ifndef QT_CODE_AREA_H
#define QT_CODE_AREA_H

#include <vector>
#include <memory>

#include <QPlainTextEdit>

#include "utility/types.h"

class QPaintEvent;
class QResizeEvent;
class QSize;
class QtCodeFile;
class QtCodeSnippet;
class QtHighlighter;
class QWidget;
class TokenLocation;
class TokenLocationFile;

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
		void paintEvent(QPaintEvent* event);

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
	virtual void mouseMoveEvent(QMouseEvent* event);

private slots:
	void updateLineNumberAreaWidth(int newBlockCount);
	void updateLineNumberArea(const QRect &, int);
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
	void createAnnotations(std::shared_ptr<TokenLocationFile> locationFile);
	void annotateText();

	void setHoveredAnnotation(const Annotation* annotation);

	bool locationBelongsToSnippet(TokenLocation* location) const;

	int toTextEditPosition(int lineNumber, int columnNumber) const;
	int startTextEditPosition() const;
	int endTextEditPosition() const;

	QtCodeFile* m_fileWidget;

	QWidget* m_lineNumberArea;
	QtHighlighter* m_highlighter;

	const uint m_startLineNumber;

	std::shared_ptr<TokenLocationFile> m_locationFile;

	std::vector<Annotation> m_annotations;
	std::vector<ScopeAnnotation> m_scopeAnnotations;

	const Annotation* m_hoveredAnnotation;

	int m_digits;
};

#endif // QT_CODE_AREA_H
