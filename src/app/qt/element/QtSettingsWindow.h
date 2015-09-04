#ifndef QT_SETTINGS_WINDOW_H
#define QT_SETTINGS_WINDOW_H

#include <QResizeEvent>
#include <QWidget>

class QtSettingsWindow
	: public QWidget
{
	Q_OBJECT

public:
	QtSettingsWindow(QWidget* parent = 0, int displacement = 0);

	QSize sizeHint() const Q_DECL_OVERRIDE;

	virtual void setup() = 0;

protected:
	void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	QWidget* m_window;

private:
	int m_displacment;
	QPoint m_dragPosition;
	bool m_mousePressedInWindow;
};

#endif //QT_SETTINGS_WINDOW_H
