#ifndef QT_CODE_AREA_H
#define QT_CODE_AREA_H

#include <memory>
#include <vector>

#include "QtCodeField.h"
#include "QtScrollSpeedChangeListener.h"

class QDragMoveEvent;
class QPaintEvent;
class QResizeEvent;
class QSize;
class QtCodeNavigator;
class QWidget;
class QtCodeArea;

class MouseWheelOverScrollbarFilter: public QObject
{
	Q_OBJECT

public:
	MouseWheelOverScrollbarFilter();

protected:
	bool eventFilter(QObject* obj, QEvent* event);
};

class QtLineNumberArea: public QWidget
{
	Q_OBJECT
public:
	QtLineNumberArea(QtCodeArea* codeArea);
	virtual ~QtLineNumberArea();

	QSize sizeHint() const override;

protected:
	virtual void paintEvent(QPaintEvent* event) override;

private:
	QtCodeArea* m_codeArea;
};


class QtCodeArea: public QtCodeField
{
	Q_OBJECT

public:
	QtCodeArea(
		size_t startLineNumber,
		const std::string& code,
		std::shared_ptr<SourceLocationFile> locationFile,
		QtCodeNavigator* navigator,
		bool showLineNumbers,
		QWidget* parent = nullptr);
	virtual ~QtCodeArea();

	virtual QSize sizeHint() const override;

	void lineNumberAreaPaintEvent(QPaintEvent* event);
	int lineNumberDigits() const;
	int lineNumberAreaWidth() const;
	void updateLineNumberAreaWidthForDigits(int digits);

	void updateSourceLocations(std::shared_ptr<SourceLocationFile> locationFile);
	void updateContent();

	void setIsActiveFile(bool isActiveFile);

	size_t getLineNumberForLocationId(Id locationId) const;
	std::pair<size_t, size_t> getLineNumbersForLocationId(Id locationId) const;

	Id getLocationIdOfFirstActiveLocation(Id tokenId) const;
	Id getLocationIdOfFirstActiveScopeLocation(Id tokenId) const;
	Id getLocationIdOfFirstHighlightedLocation() const;

	size_t getActiveLocationCount() const;

	QRectF getLineRectForLineNumber(size_t lineNumber) const;

	void findScreenMatches(
		const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches);
	void clearScreenMatches();

	void ensureLocationIdVisible(Id locationId, int parentWidth, bool animated);

protected:
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void mouseReleaseEvent(QMouseEvent* event) override;
	virtual void mousePressEvent(QMouseEvent* event) override;
	virtual void mouseMoveEvent(QMouseEvent* event) override;
	virtual void wheelEvent(QWheelEvent* event) override;

	virtual void contextMenuEvent(QContextMenuEvent* event) override;

	virtual void focusTokenIds(const std::vector<Id>& tokenIds) override;
	virtual void defocusTokenIds(const std::vector<Id>& tokenIds) override;

private slots:
	void updateLineNumberAreaWidth(int newBlockCount = 0);
	void updateLineNumberArea(const QRect&, int);
	void setIDECursorPosition();
	void setCopyAvailable(bool yes);

private:
	void clearSelection();
	void setNewTextCursor(const QTextCursor& cursor);
	void dragSelectedText();
	bool isSelectionPosition(QPoint positionPoint) const;

	void activateAnnotationsOrErrors(const std::vector<const Annotation*>& annotations);

	void annotateText();

	void createActions();

	QtCodeNavigator* m_navigator;
	QWidget* m_lineNumberArea;

	int m_digits;

	bool m_isSelecting;
	bool m_isPanning;
	bool m_isDragging;
	QPoint m_oldMousePosition;
	int m_panningDistance;

	QAction* m_copyAction;
	QAction* m_setIDECursorPositionAction;
	QPoint m_eventPosition;	   // is needed for IDE cursor control via context menu
							   // the position where the context menu is opened needs to be stored]

	bool m_isActiveFile;
	bool m_showLineNumbers;

	QtScrollSpeedChangeListener m_scrollSpeedChangeListener;
};

#endif	  // QT_CODE_AREA_H
