#ifndef QTSETTINGSWINDOW_H
#define QTSETTINGSWINDOW_H

#include <QWidget>
#include <QResizeEvent>

class QtSettingsWindow : public QWidget
{
	Q_OBJECT

public:
	QtSettingsWindow(QWidget* parent = 0, int displacement = 0);
	QSize sizeHint() const Q_DECL_OVERRIDE;
  virtual void setup() = 0;

protected:
	QWidget* m_window;

	void keyPressEvent(QKeyEvent* event);
	void resizeEvent(QResizeEvent* event);
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
private:
	int m_displacment;
	QPoint m_dragPosition;
	bool m_mousePressedInWindow;
	//void mouse
};

#endif //QTSETTINGSWINDOW_H
