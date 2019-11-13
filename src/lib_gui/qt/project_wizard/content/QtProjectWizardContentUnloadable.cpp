#include "QtProjectWizardContentUnloadable.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>
#include <boost/filesystem/path.hpp>

#include "FileSystem.h"
#include "ProjectSettings.h"
#include "SourceGroupSettingsCustomCommand.h"
#include "SqliteIndexStorage.h"

QtProjectWizardContentUnloadable::QtProjectWizardContentUnloadable(
	std::shared_ptr<SourceGroupSettingsUnloadable> settings,
	QtProjectWizardWindow* window
)
	: QtProjectWizardContent(window)
	, m_settings(settings)
{
}

void QtProjectWizardContentUnloadable::populate(QGridLayout* layout, int& row)
{
	QHBoxLayout* layoutHorz = new QHBoxLayout();
	layout->addLayout(
		layoutHorz,
		row, QtProjectWizardWindow::FRONT_COL,
		1, 1 + QtProjectWizardWindow::BACK_COL - QtProjectWizardWindow::FRONT_COL,
		Qt::AlignTop
	);

	layoutHorz->addSpacing(60);

	QLabel* infoLabel = new QLabel(
		"<p>The selected item uses a Source Group type that is not supportetd by this version of Sourcetrail.</p>"
	);
	infoLabel->setObjectName("info");
	infoLabel->setWordWrap(true);
	layoutHorz->addWidget(infoLabel);

	layoutHorz->addSpacing(40);

	row++;
}
