#ifndef QT_LICENSE_WINDOW_H
#define QT_LICENSE_WINDOW_H

#include "QtWindow.h"

class QtLicenseWindow
	: public QtWindow
{
	Q_OBJECT
public:
	QtLicenseWindow(QWidget* parent = 0);
	QSize sizeHint() const override;

protected:
	// QtWindow implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;
};

#endif // QT_LICENSE_WINDOW_H
