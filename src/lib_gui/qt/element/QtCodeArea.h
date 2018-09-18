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

class MouseWheelOverScrollbarFilter
	: public QObject
{
	Q_OBJECT

public:
	MouseWheelOverScrollbarFilter();

protected:
	bool eventFilter(QObject* obj, QEvent* event);
};

class QtLineNumberArea
	: public QWidget
{
	Q_OBJECT
public:
	QtLineNumberArea(QtCodeArea* codeArea);
	virtual ~QtLineNumberArea();

	QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
	virtual void paintEvent(QPaintEvent* event) Q_DECL_OVERRIDE;

private:
	QtCodeArea* m_codeArea;
};


class QtCodeArea
	: public QtCodeField
{
	Q_OBJECT

public:

	QtCodeArea(
		uint startLineNumber,
		const std::string& code,
		std::shared_ptr<SourceLocationFile> locationFile,
		QtCodeNavigator* navigator,
		bool showLineNumbers,
		QWidget* parent = nullptr
	);
	virtual ~QtCodeArea();

	virtual QSize sizeHint() const Q_DECL_OVERRIDE;

	void lineNumberAreaPaintEvent(QPaintEvent* event);
	int lineNumberDigits() const;
	int lineNumberAreaWidth() const;
	void updateLineNumberAreaWidthForDigits(int digits);

	void updateSourceLocations(std::shared_ptr<SourceLocationFile> locationFile);
	void updateContent();

	void setIsActiveFile(bool isActiveFile);

	uint getLineNumberForLocationId(Id locationId) const;
	std::pair<uint, uint> getLineNumbersForLocationId(Id locationId) const;

	Id getLocationIdOfFirstActiveLocation(Id tokenId) const;
	Id getLocationIdOfFirstActiveScopeLocation(Id tokenId) const;
	Id getLocationIdOfFirstHighlightedLocation() const;

	std::vector<Id> getLocationIdsForTokenIds(const std::set<Id>& tokenIds) const;

	size_t getActiveLocationCount() const;

	QRectF getLineRectForLineNumber(uint lineNumber) const;

	void findScreenMatches(const std::wstring& query, std::vector<std::pair<QtCodeArea*, Id>>* screenMatches);
	void clearScreenMatches();

	void ensureLocationIdVisible(Id locationId, int parentWidth, bool animated);

protected:
	virtual void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	virtual void wheelEvent(QWheelEvent *event) Q_DECL_OVERRIDE;

	virtual void contextMenuEvent(QContextMenuEvent* event) Q_DECL_OVERRIDE;

	virtual void focusTokenIds(const std::vector<Id>& tokenIds) override;
	virtual void defocusTokenIds(const std::vector<Id>& tokenIds) override;

private slots:
	void updateLineNumberAreaWidth(int newBlockCount = 0);
	void updateLineNumberArea(const QRect&, int);
	void clearSelection();
	void setNewTextCursor(const QTextCursor& cursor);
	void setIDECursorPosition();

private:
	void activateErrors(const std::vector<const Annotation*>& annotations);

	void annotateText();

	void createActions();

	QtCodeNavigator* m_navigator;
	QWidget* m_lineNumberArea;

	int m_digits;

	bool m_isSelecting;
	bool m_isPanning;
	QPoint m_oldMousePosition;
	int m_panningDistance;

	QAction* m_setIDECursorPositionAction;
	QPoint m_eventPosition; // is needed for IDE cursor control via context menu
							// the position where the context menu is opened needs to be stored]

	bool m_isActiveFile;
	bool m_showLineNumbers;

	QtScrollSpeedChangeListener m_scrollSpeedChangeListener;
};

#endif // QT_CODE_AREA_H
