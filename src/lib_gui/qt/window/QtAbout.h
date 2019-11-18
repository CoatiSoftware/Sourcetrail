#ifndef QT_ABOUT_H
#define QT_ABOUT_H

#include "QtWindow.h"

class QtAbout: public QtWindow
{
	Q_OBJECT

public:
	QtAbout(QWidget* parent = 0);
	QSize sizeHint() const override;

	void setupAbout();
};

#endif	  // QT_ABOUT_H
