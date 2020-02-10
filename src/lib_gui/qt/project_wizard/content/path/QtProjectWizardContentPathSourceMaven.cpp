#include "QtProjectWizardContentPathSourceMaven.h"

#include <QCheckBox>

#include "Application.h"
#include "ApplicationSettings.h"
#include "MessageStatus.h"
#include "QtDialogView.h"
#include "ScopedFunctor.h"
#include "SourceGroupJavaMaven.h"
#include "SourceGroupSettingsJavaMaven.h"
#include "logging.h"
#include "utility.h"
#include "utilityFile.h"
#include "utilityMaven.h"

QtProjectWizardContentPathSourceMaven::QtProjectWizardContentPathSourceMaven(
	std::shared_ptr<SourceGroupSettingsJavaMaven> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContentPath(window), m_settings(settings)
{
	setTitleString("Maven Project File (pom.xml)");
	setHelpString(
		"Enter the path to the main pom.xml file of your Maven project.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.");
	setFileEndings({L".xml"});
	setIsRequired(true);
}

void QtProjectWizardContentPathSourceMaven::populate(QGridLayout* layout, int& row)
{
	QtProjectWizardContentPath::populate(layout, row);
	m_picker->setPickDirectory(false);
	m_picker->setFileFilter("POM File (pom.xml)");

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

void QtProjectWizardContentPathSourceMaven::load()
{
	m_picker->setText(QString::fromStdWString(m_settings->getMavenProjectFilePath().wstr()));
	m_shouldIndexTests->setChecked(m_settings->getShouldIndexMavenTests());
}

void QtProjectWizardContentPathSourceMaven::save()
{
	m_settings->setMavenProjectFilePath(FilePath(m_picker->getText().toStdWString()));
	m_settings->setShouldIndexMavenTests(m_shouldIndexTests->isChecked());
}

std::vector<FilePath> QtProjectWizardContentPathSourceMaven::getFilePaths() const
{
	{
		const FilePath mavenPath = ApplicationSettings::getInstance()->getMavenPath();
		const FilePath mavenSettingsPath = m_settings->getMavenSettingsFilePathExpandedAndAbsolute();
		const FilePath mavenProjectRoot =
			m_settings->getMavenProjectFilePathExpandedAndAbsolute().getParentDirectory();

		if (!mavenProjectRoot.exists())
		{
			LOG_INFO(
				"Could not find any source file paths because Maven project path does not exist.");
			return std::vector<FilePath>();
		}

		QtDialogView* dialogView = dynamic_cast<QtDialogView*>(
			Application::getInstance()->getDialogView(DialogView::UseCase::PROJECT_SETUP).get());

		ScopedFunctor scopedFunctor([&dialogView]() { dialogView->hideUnknownProgressDialog(); });

		dialogView->setParentWindow(m_window);
		dialogView->showUnknownProgressDialog(
			L"Preparing Project", L"Maven\nGenerating Source Files");

		const std::wstring errorMessage = utility::mavenGenerateSources(
			mavenPath, mavenSettingsPath, mavenProjectRoot);
		if (!errorMessage.empty())
		{
			MessageStatus(errorMessage, true, false).dispatch();
			Application::getInstance()->handleDialog(errorMessage);
			return std::vector<FilePath>();
		}
	}

	return utility::getAsRelativeIfShorter(
		utility::toVector(SourceGroupJavaMaven(m_settings).getAllSourceFilePaths()),
		m_settings->getProjectDirectoryPath());
}

std::shared_ptr<SourceGroupSettings> QtProjectWizardContentPathSourceMaven::getSourceGroupSettings()
{
	return m_settings;
}
