#ifndef QT_PROJECT_WIZARD_CONTENT_REQUIRED_LABEL_H
#define QT_PROJECT_WIZARD_CONTENT_REQUIRED_LABEL_H

#include "QtProjectWizardContent.h"

class QtProjectWizardContentRequiredLabel: public QtProjectWizardContent
{
public:
	QtProjectWizardContentRequiredLabel(QtProjectWizardWindow* window)
		: QtProjectWizardContent(window)
	{}

	// QtProjectWizardContent implementation
	void populate(QGridLayout* layout, int& row) override
	{
		QLabel* label = createFormLabel(QStringLiteral("* required"));
		layout->addWidget(label, row, QtProjectWizardWindow::FRONT_COL, Qt::AlignTop);
		row++;
	}
};

#endif	  // QT_PROJECT_WIZARD_CONTENT_REQUIRED_LABEL_H
