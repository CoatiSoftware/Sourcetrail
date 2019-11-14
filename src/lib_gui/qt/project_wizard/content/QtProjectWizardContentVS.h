#ifndef QT_PROJECT_WIZARD_CONTENT_VS_H
#define QT_PROJECT_WIZARD_CONTENT_VS_H

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

#endif // QT_PROJECT_WIZARD_CONTENT_VS_H
