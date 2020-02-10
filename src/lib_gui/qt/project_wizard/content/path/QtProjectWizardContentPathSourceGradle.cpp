#include "QtProjectWizardContentPathSourceGradle.h"

#include <QCheckBox>

#include "Application.h"
#include "QtDialogView.h"
#include "SourceGroupJavaGradle.h"
#include "SourceGroupSettingsJavaGradle.h"
#include "utility.h"
#include "utilityFile.h"

QtProjectWizardContentPathSourceGradle::QtProjectWizardContentPathSourceGradle(
	std::shared_ptr<SourceGroupSettingsJavaGradle> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContentPath(window), m_settings(settings)
{
	setTitleString("Gradle Project File (build.gradle)");
	setHelpString(
		"Enter the path to the main build.gradle file of your Gradle project.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.");
	setFileEndings({L".gradle"});
	setIsRequired(true);
}

void QtProjectWizardContentPathSourceGradle::populate(QGridLayout* layout, int& row)
{
	QtProjectWizardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("Gradle Build File (build.gradle)");

	QPushButton* filesButton = addFilesButton("show source files", nullptr, row);
	m_shouldIndexTests = new QCheckBox("Should Index Tests");

	QHBoxLayout* hlayout = new QHBoxLayout();
	hlayout->setContentsMargins(0, 0, 0, 0);
	hlayout->addWidget(m_shouldIndexTests);
	hlayout->addStretch();
	hlayout->addWidget(filesButton);

	layout->addLayout(hlayout, row, QtProjectWizardWindow::BACK_COL);
	row++;
}

void QtProjectWizardContentPathSourceGradle::load()
{
	m_picker->setText(QString::fromStdWString(m_settings->getGradleProjectFilePath().wstr()));
	m_shouldIndexTests->setChecked(m_settings->getShouldIndexGradleTests());
}

void QtProjectWizardContentPathSourceGradle::save()
{
	m_settings->setGradleProjectFilePath(FilePath(m_picker->getText().toStdWString()));
	m_settings->setShouldIndexGradleTests(m_shouldIndexTests->isChecked());
}

std::vector<FilePath> QtProjectWizardContentPathSourceGradle::getFilePaths() const
{
	QtDialogView* dialogView = dynamic_cast<QtDialogView*>(
		Application::getInstance()->getDialogView(DialogView::UseCase::PROJECT_SETUP).get());
	dialogView->setParentWindow(m_window);

	return utility::getAsRelativeIfShorter(
		utility::toVector(SourceGroupJavaGradle(m_settings).getAllSourceFilePaths()),
		m_settings->getProjectDirectoryPath());
}

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathSourceGradle::getSourceGroupSettings()
{
	return m_settings;
}
