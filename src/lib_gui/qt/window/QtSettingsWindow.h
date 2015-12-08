#ifndef QT_SETTINGS_WINDOW_H
#define QT_SETTINGS_WINDOW_H

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QWidget>

class QFormLayout;
class QLabel;
class QPushButton;

class QtSettingsWindow
	: public QWidget
{
	Q_OBJECT

public:
	QtSettingsWindow(QWidget* parent = 0, int displacement = 0);

	QSize sizeHint() const Q_DECL_OVERRIDE;

	virtual void setup() = 0;

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
	virtual void populateForm(QFormLayout* layout);

	void updateTitle(QString title);
	void updateDoneButton(QString text);
	void hideCancelButton(bool hidden);

	QWidget* createLabelWithHelpButton(QString name, QPushButton** helpButton);

	void showHelpMessage(const QString& msg);

	QWidget* m_window;

	QLabel* m_title;

	QPushButton* m_cancelButton;
	QPushButton* m_doneButton;

	QHBoxLayout* m_buttonsLayout;

private:
	int m_displacment;
	QPoint m_dragPosition;
	bool m_mousePressedInWindow;
};

#endif //QT_SETTINGS_WINDOW_H
