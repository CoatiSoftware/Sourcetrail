#ifndef QT_GRAPHICS_VIEW_H
#define QT_GRAPHICS_VIEW_H

#include <QGraphicsView>

class QtGraphicsView
	: public QGraphicsView
{
	Q_OBJECT
	Q_PROPERTY(qreal zoom READ zoom WRITE setZoom)

public:
	QtGraphicsView(QWidget* parent);

	float getZoomFactor() const;
	void setAppZoomFactor(float appZoomFactor);

	qreal zoom() const;
	void setZoom(qreal zoom);

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void keyPressEvent(QKeyEvent* event);
	void wheelEvent(QWheelEvent* event);

signals:
	void emptySpaceClicked();

private:
	void updateTransform();
	void updateZoom(bool in);

	QPoint m_last;

	float m_zoomFactor;
	float m_appZoomFactor;
};

#endif // QT_GRAPHICS_VIEW_H
