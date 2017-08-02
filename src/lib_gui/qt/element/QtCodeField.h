#ifndef QT_CODE_FIELD_H
#define QT_CODE_FIELD_H

#include <memory>
#include <set>

#include <QPlainTextEdit>

#include "data/location/LocationType.h"
#include "utility/types.h"

class QtHighlighter;
class SourceLocation;
class SourceLocationFile;

class QtCodeField
	: public QPlainTextEdit
{
	Q_OBJECT

public:
	static void clearAnnotationColors();

	QtCodeField(
		uint startLineNumber,
		const std::string& code,
		std::shared_ptr<SourceLocationFile> locationFile,
		QWidget* parent = nullptr);
	~QtCodeField();

	virtual QSize sizeHint() const Q_DECL_OVERRIDE;

	uint getStartLineNumber() const;
	uint getEndLineNumber() const;

	std::string getCode() const;

	std::shared_ptr<SourceLocationFile> getSourceLocationFile() const;

	void annotateText();

protected:
	virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;
	virtual void enterEvent(QEvent* event) Q_DECL_OVERRIDE;
	virtual void leaveEvent(QEvent* event) Q_DECL_OVERRIDE;

	virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

	virtual void focusTokenIds(const std::vector<Id>& tokenIds);
	virtual void defocusTokenIds(const std::vector<Id>& tokenIds);

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

	bool annotateText(
		const std::set<Id>& activeSymbolIds, const std::set<Id>& activeLocationIds, const std::set<Id>& focusedSymbolIds);

	void createAnnotations(std::shared_ptr<SourceLocationFile> locationFile);
	void activateAnnotations(const std::vector<const Annotation*>& annotations);

	int toTextEditPosition(int lineNumber, int columnNumber) const;
	std::pair<int, int> toLineColumn(int textEditPosition) const;

	int startTextEditPosition() const;
	int endTextEditPosition() const;

	void setHoveredAnnotations(const std::vector<const Annotation*>& annotations);
	std::vector<QRect> getCursorRectsForAnnotation(const Annotation& annotation) const;

	const AnnotationColor& getAnnotationColorForAnnotation(const Annotation& annotation);
	void setTextColorForAnnotation(Annotation& annotation, QColor color) const;

	std::vector<const Annotation*> getInteractiveAnnotationsForPosition(int pos) const;

	std::vector<Annotation> m_annotations;
	std::vector<const Annotation*> m_hoveredAnnotations;

private:
	static std::vector<AnnotationColor> s_annotationColors;

	void createLineLengthCache();

	const uint m_startLineNumber;
	const std::string m_code;

	std::shared_ptr<SourceLocationFile> m_locationFile;

	QtHighlighter* m_highlighter;

	std::vector<int> m_lineLengths;
	std::set<size_t> m_colorChangedAnnotationIndices;

	int m_endTextEditPosition;
	bool m_wasAnnotated;
};

#endif // QT_CODE_FIELD_H
