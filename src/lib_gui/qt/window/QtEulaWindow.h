#ifndef QT_EULA_LICENSE_H
#define QT_EULA_LICENSE_H

#include "QtWindow.h"

class QtEulaWindow
	: public QtWindow
{
	Q_OBJECT

public:
	QtEulaWindow(QWidget* parent, bool forceAccept);
	QSize sizeHint() const override;

protected:
	// QtWindow implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;

private:
	bool m_forceAccept;
};

#endif // QT_EULA_LICENSE_H
