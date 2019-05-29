#ifndef QT_PROJECT_WIZARD_CONTENT_SOURCE_GROUP_INFO_TEXT_H
#define QT_PROJECT_WIZARD_CONTENT_SOURCE_GROUP_INFO_TEXT_H

#include "QtProjectWizardContent.h"

class QtProjectWizardContentSourceGroupInfoText
	: public QtProjectWizardContent
{
	Q_OBJECT

public:
	QtProjectWizardContentSourceGroupInfoText(QtProjectWizardWindow* window);

	// QtProjectWizardContent implementation
	virtual void populate(QGridLayout* layout, int& row) override;
};

#endif // QT_PROJECT_WIZARD_CONTENT_SOURCE_GROUP_INFO_TEXT_H
