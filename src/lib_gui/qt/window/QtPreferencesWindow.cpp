#include "qt/window/QtPreferencesWindow.h"

#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessagePluginPortChange.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageScrollSpeedChange.h"

#include "Application.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPreferences.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentSummary.h"
#include "utility/utilityApp.h"

QtPreferencesWindow::QtPreferencesWindow(QWidget* parent)
	: QtProjectWizzardWindow(parent)
{
	// save old application settings so they can be compared later
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	m_appSettings.setHeaderSearchPaths(appSettings->getHeaderSearchPaths());
	m_appSettings.setFrameworkSearchPaths(appSettings->getFrameworkSearchPaths());
	m_appSettings.setScrollSpeed(appSettings->getScrollSpeed());
	m_appSettings.setSourcetrailPort(appSettings->getSourcetrailPort());
	m_appSettings.setPluginPort(appSettings->getPluginPort());


	QtProjectWizzardContentSummary* summary = new QtProjectWizzardContentSummary(this);
	summary->setIsForm(true);
	summary->addContent(new QtProjectWizzardContentPreferences(this));

	summary->addContent(new QtProjectWizzardContentPathsHeaderSearchGlobal(this));
	if (utility::getOsType() == OS_MAC)
	{
		summary->addContent(new QtProjectWizzardContentPathsFrameworkSearchGlobal(this));
	}

	setPreferredSize(QSize(750, 500));
	setContent(summary);
	setScrollAble(content()->isScrollAble());
}

QtPreferencesWindow::~QtPreferencesWindow()
{
}

void QtPreferencesWindow::windowReady()
{
	QtProjectWizzardWindow::windowReady();

	updateTitle("PREFERENCES");
	updateNextButton("Save");
	setPreviousVisible(false);

	loadContent();
}

void QtPreferencesWindow::handleNext()
{
	if (!content()->check())
	{
		return;
	}

	saveContent();

	bool appSettingsChanged = !(m_appSettings == *ApplicationSettings::getInstance().get());

	if (m_appSettings.getScrollSpeed() != ApplicationSettings::getInstance()->getScrollSpeed())
	{
		MessageScrollSpeedChange(ApplicationSettings::getInstance()->getScrollSpeed()).dispatch();
	}

	if (m_appSettings.getSourcetrailPort() != ApplicationSettings::getInstance()->getSourcetrailPort() ||
		m_appSettings.getPluginPort() != ApplicationSettings::getInstance()->getPluginPort())
	{
		MessagePluginPortChange().dispatch();
	}

	Application::getInstance()->loadSettings();

	if (appSettingsChanged)
	{
		Project* currentProject = Application::getInstance()->getCurrentProject().get();
		if (currentProject)
		{
			MessageLoadProject(currentProject->getProjectSettingsFilePath(), true).dispatch();
		}
	}
	else
	{
		MessageRefresh().refreshUiOnly().dispatch();
	}

	QtWindow::handleClose();
}
