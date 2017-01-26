#ifndef QT_GRAPHICS_VIEW_H
#define QT_GRAPHICS_VIEW_H

#include <functional>
#include <memory>

#include <QGraphicsView>

#include "utility/file/FilePath.h"

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

	void setMouseWheelCallback(const std::function<void(QWheelEvent*)>& callback);
	void updateZoom(float delta);

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

	std::function<void(QWheelEvent*)> m_mouseWheelCallback;
};

#endif // QT_GRAPHICS_VIEW_H
