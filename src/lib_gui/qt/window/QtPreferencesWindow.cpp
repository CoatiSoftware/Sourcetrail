#include "qt/window/QtPreferencesWindow.h"

#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessagePluginPortChange.h"
#include "utility/messaging/type/MessageRefresh.h"
#include "utility/messaging/type/MessageScrollSpeedChange.h"

#include "Application.h"
#include "component/view/DialogView.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentPreferences.h"
#include "qt/window/project_wizzard/QtProjectWizzardContentGroup.h"
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

	m_appSettings.setScreenAutoScaling(appSettings->getScreenAutoScaling());
	m_appSettings.setScreenScaleFactor(appSettings->getScreenScaleFactor());


	QtProjectWizzardContentGroup* summary = new QtProjectWizzardContentGroup(this);
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

	Application* app = Application::getInstance().get();
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	bool needsRestart =
		m_appSettings.getScreenAutoScaling() != appSettings->getScreenAutoScaling() ||
		m_appSettings.getScreenScaleFactor() != appSettings->getScreenScaleFactor();

	if (needsRestart)
	{
		app->getDialogView()->confirm(
			"<p>Please restart the application for all changes to take effect.</p><p>Note: These changes may harm "
			"the execution of the application. In case the application is not useable anymore, please run the "
			"'resetPreferences.sh' script located in your install directory.</p>"
		);
	}


	bool appSettingsChanged = !(m_appSettings == *appSettings);

	if (m_appSettings.getScrollSpeed() != appSettings->getScrollSpeed())
	{
		MessageScrollSpeedChange(appSettings->getScrollSpeed()).dispatch();
	}

	if (m_appSettings.getSourcetrailPort() != appSettings->getSourcetrailPort() ||
		m_appSettings.getPluginPort() != appSettings->getPluginPort())
	{
		MessagePluginPortChange().dispatch();
	}

	app->loadSettings();

	if (appSettingsChanged)
	{
		Project* currentProject = app->getCurrentProject().get();
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
