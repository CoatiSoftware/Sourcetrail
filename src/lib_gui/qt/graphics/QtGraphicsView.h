#ifndef QT_GRAPHICS_VIEW_H
#define QT_GRAPHICS_VIEW_H

#include <memory>

#include <QGraphicsView>

#include <memory>

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

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);

	void wheelEvent(QWheelEvent* event);

	void contextMenuEvent(QContextMenuEvent* event);

signals:
	void emptySpaceClicked();

private slots:
	void updateTimer();
	void stopTimer();

	void exportGraph();
	void copyNodeName();

private:
	bool moves() const;

	void updateTransform();
	void updateZoom(float delta);

	QPoint m_last;

	float m_zoomFactor;
	float m_appZoomFactor;

	bool m_up;
	bool m_down;
	bool m_left;
	bool m_right;
	bool m_shift;

	std::string m_clipboardNodeName;

	std::shared_ptr<QTimer> m_timer;
	std::shared_ptr<QTimer> m_timerStopper;

	QAction* m_exportGraphAction;
	QAction* m_copyNodeNameAction;
};

#endif // QT_GRAPHICS_VIEW_H
