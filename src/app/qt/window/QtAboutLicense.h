#ifndef QT_ABOUT_LICENSE_H
#define QT_ABOUT_LICENSE_H

#include <QPushButton>
#include <QWidget>

#include "qt/window/QtSettingsWindow.h"

class QtAboutLicense
	: public QtSettingsWindow
{
	Q_OBJECT

public:
	QtAboutLicense(QWidget* parent = 0);
	QSize sizeHint() const Q_DECL_OVERRIDE;

	virtual void setup() override;

protected:
	virtual void populateForm(QFormLayout* layout) override;

private slots:
	void handleCancelButtonPress();
	void handleUpdateButtonPress();
};

#endif //QT_ABOUT_LICENSE_H
