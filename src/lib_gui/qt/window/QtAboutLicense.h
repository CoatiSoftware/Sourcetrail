#ifndef QT_ABOUT_LICENSE_H
#define QT_ABOUT_LICENSE_H

#include "qt/window/QtWindow.h"

class QtAboutLicense
	: public QtWindow
{
public:
	QtAboutLicense(QWidget* parent = 0);
	QSize sizeHint() const override;

protected:
	// QtWindow implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;
};

#endif // QT_ABOUT_LICENSE_H
