#ifndef QT_GRAPHICS_VIEW_H
#define QT_GRAPHICS_VIEW_H

#include <memory>

#include <QGraphicsView>
#include <QPushButton>

#include "utility/types.h"

class QTimer;
class QtGraphNode;

class QtGraphicsView
	: public QGraphicsView
{
	Q_OBJECT

public:
	QtGraphicsView(QWidget* parent);

	float getZoomFactor() const;
	void setAppZoomFactor(float appZoomFactor);

	QtGraphNode* getNodeAtCursorPosition() const;

	void ensureVisibleAnimated(const QRectF& rect, int xmargin = 50, int ymargin = 50);

	void updateZoom(float delta);

	void refreshStyle();

protected:
	void resizeEvent(QResizeEvent* event);

	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);

	void wheelEvent(QWheelEvent* event);

	void contextMenuEvent(QContextMenuEvent* event);

signals:
	void emptySpaceClicked();
	void characterKeyPressed(QChar c);
	void resized();

private slots:
	void updateTimer();
	void stopTimer();

	void exportGraph();
	void copyNodeName();
	void bookmarkNode();

	void zoomInPressed();
	void zoomOutPressed();

	void hideZoomLabel();

private:
	bool moves() const;

	void setZoomFactor(float zoomFactor);
	void updateTransform();

	QPoint m_last;

	float m_zoomFactor;
	float m_appZoomFactor;

	bool m_up;
	bool m_down;
	bool m_left;
	bool m_right;
	bool m_shift;

	std::string m_clipboardNodeName;
	Id m_bookmarkNodeId;

	std::shared_ptr<QTimer> m_timer;
	std::shared_ptr<QTimer> m_timerStopper;
	std::shared_ptr<QTimer> m_zoomLabelTimer;

	QAction* m_exportGraphAction;
	QAction* m_copyNodeNameAction;
	QAction* m_bookmarkNodeAction;

	QPushButton* m_zoomState;
	QPushButton* m_zoomInButton;
	QPushButton* m_zoomOutButton;

	float m_zoomInButtonSpeed;
	float m_zoomOutButtonSpeed;
};

#endif // QT_GRAPHICS_VIEW_H
