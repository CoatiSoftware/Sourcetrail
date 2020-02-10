#include "QtProjectWizardContentPathsFrameworkSearch.h"

#include "SourceGroupSettings.h"
#include "SourceGroupSettingsWithCxxPathsAndFlags.h"

QtProjectWizardContentPathsFrameworkSearch::QtProjectWizardContentPathsFrameworkSearch(
	std::shared_ptr<SourceGroupSettings> settings,
	QtProjectWizardWindow* window,
	bool indicateAsAdditional)
	: QtProjectWizardContentPaths(settings, window, QtPathListBox::SELECTION_POLICY_DIRECTORIES_ONLY, true)
{
	setTitleString(
		indicateAsAdditional ? "Additional Framework Search Paths" : "Framework Search Paths");
	setHelpString(
		"Framework Search Paths define where MacOS framework containers (.framework), that your "
		"project depends on, are "
		"found. These paths are usually passed to the compiler with the '-iframework' flag.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}.");
}

void QtProjectWizardContentPathsFrameworkSearch::load()
{
	std::shared_ptr<SourceGroupSettingsWithCxxPathsAndFlags> cxxSettings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithCxxPathsAndFlags>(m_settings);
	if (cxxSettings)
	{
		m_list->setPaths(cxxSettings->getFrameworkSearchPaths());
	}
}

void QtProjectWizardContentPathsFrameworkSearch::save()
{
	std::shared_ptr<SourceGroupSettingsWithCxxPathsAndFlags> cxxSettings =
		std::dynamic_pointer_cast<SourceGroupSettingsWithCxxPathsAndFlags>(m_settings);
	if (cxxSettings)
	{
		cxxSettings->setFrameworkSearchPaths(m_list->getPathsAsDisplayed());
	}
}
