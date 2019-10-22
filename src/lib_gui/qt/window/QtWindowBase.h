#ifndef QT_WINDOW_BASE_H
#define QT_WINDOW_BASE_H

#include <QSizeGrip>
#include <QWidget>

#include "QtWindowStack.h"

class QLabel;
class QPushButton;
class QVBoxLayout;
class QMouseEvent;

class QtWindowBase
	: public QtWindowStackElement
{
	Q_OBJECT

public:
	QtWindowBase(bool isSubWindow, QWidget* parent);

	QSize sizeHint() const override;

	void setSizeGripStyle(bool isBlack);

	bool isSubWindow() const;

	void moveToCenter();

	// QtWindowStackElement implementation
	virtual void showWindow() override;
	virtual void hideWindow() override;

protected:
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

	bool m_isSubWindow;

	QWidget* m_window;
	QWidget* m_content;

	QPoint m_dragPosition;
	bool m_mousePressedInWindow;

	QSizeGrip* m_sizeGrip;
};

#endif // QT_WINDOW_BASE_H
