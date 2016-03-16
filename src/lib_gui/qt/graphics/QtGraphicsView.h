#ifndef QT_GRAPHICS_VIEW_H
#define QT_GRAPHICS_VIEW_H

#include <QGraphicsView>

class QTimer;

class QtGraphicsView
	: public QGraphicsView
{
	Q_OBJECT

public:
	QtGraphicsView(QWidget* parent);

	float getZoomFactor() const;
	void setAppZoomFactor(float appZoomFactor);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void keyPressEvent(QKeyEvent* event);
	void keyReleaseEvent(QKeyEvent* event);

	void wheelEvent(QWheelEvent* event);

signals:
	void emptySpaceClicked();

private slots:
	void update();

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

	std::shared_ptr<QTimer> m_timer;
};

#endif // QT_GRAPHICS_VIEW_H
