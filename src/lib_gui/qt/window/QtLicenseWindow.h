#ifndef QT_LICENSE_WINDOW_H
#define QT_LICENSE_WINDOW_H

#include "QtWindow.h"

class QLabel;
class QRadioButton;
class QTextEdit;

class QtLicenseWindow
	: public QtWindow
{
	Q_OBJECT

public:
	QtLicenseWindow(QWidget* parent = 0);
	QSize sizeHint() const override;

	void clear();
	void load();

	void setErrorMessage(const QString& errorMessage);

protected:
	// QtWindow implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;

	virtual void handleNext() override;

private slots:
	void optionChanged();

private:
	QRadioButton* m_commercialUse;
	QRadioButton* m_privateUse;

	QTextEdit* m_licenseText;
	QLabel* m_errorLabel;

	QtWindow* m_confirmWindow = nullptr;
};


class QtNonCommercialInfoWindow
	: public QtWindow
{
	Q_OBJECT

public:
	QtNonCommercialInfoWindow(QWidget* parent = 0);
	QSize sizeHint() const override;

protected:
	// QtWindow implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;
};

#endif // QT_LICENSE_WINDOW_H
