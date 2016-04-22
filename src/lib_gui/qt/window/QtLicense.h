#ifndef QT_LICENSE_H
#define QT_LICENSE_H

#include "qt/window/QtWindow.h"

class QLabel;
class QPushButton;
class QTextEdit;

class QtLicense
	: public QtWindow
{
	Q_OBJECT

public:
	QtLicense(QWidget* parent = 0);
	QSize sizeHint() const override;

	void clear();
	void load();

	void setErrorMessage(const QString& errorMessage);

protected:
	// QtWindow implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;

	virtual void handleNext() override;
	virtual void handleClose() override;

private:
	QTextEdit* m_licenseText;
	QLabel* m_errorLabel;
};

#endif // QT_LICENSE_H
