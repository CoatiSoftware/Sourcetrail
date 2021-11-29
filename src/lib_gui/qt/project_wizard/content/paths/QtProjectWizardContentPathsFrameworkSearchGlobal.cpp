#include "QtProjectWizardContentPathsFrameworkSearchGlobal.h"

#include "ApplicationSettings.h"
#include "utilityApp.h"
#include "utilityPathDetection.h"

QtProjectWizardContentPathsFrameworkSearchGlobal::QtProjectWizardContentPathsFrameworkSearchGlobal(
	QtProjectWizardWindow* window)
	: QtProjectWizardContentPaths(
		  std::shared_ptr<SourceGroupSettings>(),
		  window,
		  QtPathListBox::SELECTION_POLICY_DIRECTORIES_ONLY,
		  true)
{
	setTitleString(QStringLiteral("Global Framework Search Paths"));
	setHelpString(QString::fromStdString(
		"The Global Framework Search Paths will be used in all your projects - in addition to the "
		"project specific "
		"Framework Search Paths.<br />"
		"<br />"
		"They define where MacOS framework containers (.framework) are found "
		"(See <a href=\"" +
		utility::getDocumentationLink() +
		"#finding-system-header-locations\">"
		"Finding System Header Locations</a> or use the auto detection below)."));

	m_pathDetector = utility::getCxxFrameworkPathDetector();
	m_makePathsRelativeToProjectFileLocation = false;
}

void QtProjectWizardContentPathsFrameworkSearchGlobal::load()
{
	m_list->setPaths(ApplicationSettings::getInstance()->getFrameworkSearchPaths());
}

void QtProjectWizardContentPathsFrameworkSearchGlobal::save()
{
	ApplicationSettings::getInstance()->setFrameworkSearchPaths(m_list->getPathsAsDisplayed());
	ApplicationSettings::getInstance()->save();
}
