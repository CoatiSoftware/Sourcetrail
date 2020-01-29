#include "QtProjectWizardContentSourceGroupInfoText.h"

QtProjectWizardContentSourceGroupInfoText::QtProjectWizardContentSourceGroupInfoText(
	QtProjectWizardWindow* window)
	: QtProjectWizardContent(window)
{
}

void QtProjectWizardContentSourceGroupInfoText::populate(QGridLayout* layout, int& row)
{
	QHBoxLayout* layoutHorz = new QHBoxLayout();
	layout->addLayout(
		layoutHorz,
		row,
		QtProjectWizardWindow::FRONT_COL,
		1,
		1 + QtProjectWizardWindow::BACK_COL - QtProjectWizardWindow::FRONT_COL,
		Qt::AlignTop);

	layoutHorz->addSpacing(60);

	QLabel* infoLabel = new QLabel(
		"<p>Please add at least one source group to your project. A source group specifies which "
		"source files should be "
		"analyzed by Sourcetrail and includes all parameters required to analyze those source "
		"files. A Sourcetrail project "
		"may contain multiple source groups, which may be necessary if you want to analyze source "
		"files from different "
		"projects that do not share the same parameters.</p>"
		"<p><b>Hint</b>: If your project contains source code for multiple build targets, you can "
		"add all of those source "
		"files with one single source group as long as they all share the same parameters.</p>");
	infoLabel->setObjectName(QStringLiteral("info"));
	infoLabel->setWordWrap(true);
	layoutHorz->addWidget(infoLabel);

	layoutHorz->addSpacing(40);

	row++;
}