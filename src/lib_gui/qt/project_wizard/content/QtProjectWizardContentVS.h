#ifndef QT_PROJECT_WIZARD_CONTENT_VS_H
#define QT_PROJECT_WIZARD_CONTENT_VS_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "QtProjectWizardContent.h"

class QtProjectWizardContentVS
	: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentVS(QtProjectWizardWindow* window);

	virtual void populate(QGridLayout* layout, int& row) override;

private slots:
	void handleVSCDBClicked();
};

#endif // BUILD_CXX_LANGUAGE_PACKAGE

#endif // QT_PROJECT_WIZARD_CONTENT_VS_H
