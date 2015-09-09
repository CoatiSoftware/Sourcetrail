#ifndef QT_ABOUT_H
#define QT_ABOUT_H

#include <QPushButton>
#include <QWidget>

#include "qt/window/QtSettingsWindow.h"

class QtAbout
	: public QtSettingsWindow
{
	Q_OBJECT

public:
	QtAbout(QWidget* parent = 0);
	QSize sizeHint() const Q_DECL_OVERRIDE;

	virtual void setup() override;
protected:
	virtual void populateForm(QFormLayout* layout) override;

private slots:
	void handleCancelButtonPress();
	void handleUpdateButtonPress();
	void handleDoneButtonPress();

private:
	QPushButton* m_cancelButton;
	QPushButton* m_updateButton;

};

#endif //QT_ABOUT_H
