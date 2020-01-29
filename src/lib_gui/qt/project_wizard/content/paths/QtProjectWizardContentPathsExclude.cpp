#include "QtProjectWizardContentPathsExclude.h"

#include "SourceGroupSettings.h"
#include "SourceGroupSettingsWithExcludeFilters.h"
#include "utility.h"
#include "utilityString.h"

QtProjectWizardContentPathsExclude::QtProjectWizardContentPathsExclude(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContentPaths(
		  settings, window, QtPathListBox::SELECTION_POLICY_FILES_AND_DIRECTORIES, false)
{
	setTitleString(QStringLiteral("Excluded Files & Directories"));
	setHelpString(
		"<p>These paths define the files and directories that will be left out from indexing.</p>"
		"<p>Hints:"
		"<ul>"
		"<li>You can use the wildcard \"*\" which represents characters except \"\\\" or \"/\" "
		"(e.g. \"src/*/test.h\" matches \"src/app/test.h\" but does not match "
		"\"src/app/widget/test.h\" or \"src/test.h\")</li>"
		"<li>You can use the wildcard \"**\" which represents arbitrary characters (e.g. "
		"\"src**test.h\" matches \"src/app/test.h\" as well as \"src/app/widget/test.h\" or "
		"\"src/test.h\")</li>"
		"<li>You can make use of environment variables with ${ENV_VAR}</li>"
		"</ul></p>");
}

void QtProjectWizardContentPathsExclude::load()
{
	if (std::shared_ptr<SourceGroupSettingsWithExcludeFilters> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsWithExcludeFilters>(
				m_settings))	// FIXME: pass msettings as required type
	{
		m_list->setPaths(utility::convert<std::wstring, FilePath>(
			settings->getExcludeFilterStrings(), [](const std::wstring& s) { return FilePath(s); }));
	}
}

void QtProjectWizardContentPathsExclude::save()
{
	if (std::shared_ptr<SourceGroupSettingsWithExcludeFilters> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsWithExcludeFilters>(
				m_settings))	// FIXME: pass msettings as required type
	{
		settings->setExcludeFilterStrings(utility::toWStrings(m_list->getPathsAsDisplayed()));
	}
}
