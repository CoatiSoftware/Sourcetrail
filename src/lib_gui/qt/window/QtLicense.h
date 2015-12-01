#ifndef QT_LICENSE_H
#define QT_LICENSE_H

#include <QPushButton>
#include <QWidget>
#include <QtWidgets/qtextedit.h>

#include "qt/window/QtSettingsWindow.h"

class QtLicense
	: public QtSettingsWindow
{
	Q_OBJECT

public:
	QtLicense(QWidget* parent = 0);
	QSize sizeHint() const Q_DECL_OVERRIDE;

	void clear();

	virtual void setup() override;

private slots:
	void handleCancelButtonPress();
	void handleUpdateButtonPress();

private:
	QPushButton* m_cancelButton;
	QPushButton* m_updateButton;

	QTextEdit* m_licenseText;
	QLabel* m_errorLabel;
};

#endif //QT_LICENSE_H
