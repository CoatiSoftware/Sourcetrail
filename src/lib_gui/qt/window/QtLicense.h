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

	virtual void setup() override;
protected:
	virtual void populateForm(QFormLayout* layout) override;

private slots:
	void handleCancelButtonPress();
	void handleUpdateButtonPress();

private:
	QPushButton* m_cancelButton;
	QPushButton* m_updateButton;

	QTextEdit* m_licenseText;

};

#endif //QT_LICENSE_H
