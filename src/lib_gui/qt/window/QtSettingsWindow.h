#ifndef QT_SETTINGS_WINDOW_H
#define QT_SETTINGS_WINDOW_H

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QWidget>

#include "qt/window/QtWindowStack.h"

class QFormLayout;
class QLabel;
class QPushButton;
class QVBoxLayout;

class QtSettingsWindow
	: public QtWindowStackElement
{
	Q_OBJECT

public:
	QtSettingsWindow(QWidget* parent = 0, int displacement = 0);

	QSize sizeHint() const Q_DECL_OVERRIDE;

	virtual void setup() = 0;

	void setCancelAble(bool cancelAble);

	void updateTitle(QString title);
	void updateDoneButton(QString text);

	// QtWindowStackElement implementation
	virtual void showWindow() override;
	virtual void hideWindow() override;

signals:
	void finished();
	void canceled();

protected:
	void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	void resizeEvent(QResizeEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

	void setupForm();
	virtual void populateWindow(QWidget* widget);

	void addLogo();
	void showButtons(QVBoxLayout* layout);

	void hideCancelButton(bool hidden);

	QWidget* m_window;

	QLabel* m_title;

	QPushButton* m_cancelButton;
	QPushButton* m_doneButton;

	QHBoxLayout* m_buttonsLayout;

private:
	int m_displacment;
	QPoint m_dragPosition;
	bool m_mousePressedInWindow;
	bool m_cancelAble;
};

#endif //QT_SETTINGS_WINDOW_H
