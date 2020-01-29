#include "QtProjectWizardContentPathsSource.h"

#include "language_packages.h"

#include "SourceGroupCustomCommand.h"
#include "SourceGroupSettingsCustomCommand.h"
#include "SourceGroupSettingsWithSourcePaths.h"
#include "utility.h"
#include "utilityFile.h"

#if BUILD_CXX_LANGUAGE_PACKAGE
#	include "SourceGroupCxxEmpty.h"
#	include "SourceGroupSettingsWithCxxPathsAndFlags.h"
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
#	include "SourceGroupJavaEmpty.h"
#	include "SourceGroupSettingsJavaEmpty.h"
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

#if BUILD_PYTHON_LANGUAGE_PACKAGE
#	include "SourceGroupPythonEmpty.h"
#	include "SourceGroupSettingsPythonEmpty.h"
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE

QtProjectWizardContentPathsSource::QtProjectWizardContentPathsSource(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContentPaths(
		  settings, window, QtPathListBox::SELECTION_POLICY_FILES_AND_DIRECTORIES)
{
	m_showFilesString = QStringLiteral("show files");

	setTitleString(QStringLiteral("Files & Directories to Index"));
	setHelpString(QStringLiteral(
		"These paths define the files and directories that will be indexed by Sourcetrail. Provide "
		"a directory to recursively "
		"add all contained source and header files.<br />"
		"<br />"
		"If your project's source code resides in one location, but generated source files are "
		"kept at a different location, "
		"you will also need to add that directory.<br />"
		"<br />"
		"You can make use of environment variables with ${ENV_VAR}."));
}

void QtProjectWizardContentPathsSource::load()
{
	if (std::shared_ptr<SourceGroupSettingsWithSourcePaths> pathSettings =
			std::dynamic_pointer_cast<SourceGroupSettingsWithSourcePaths>(
				m_settings))	// FIXME: pass msettings as required type
	{
		m_list->setPaths(pathSettings->getSourcePaths());
	}
}

void QtProjectWizardContentPathsSource::save()
{
	if (std::shared_ptr<SourceGroupSettingsWithSourcePaths> pathSettings =
			std::dynamic_pointer_cast<SourceGroupSettingsWithSourcePaths>(
				m_settings))	// FIXME: pass msettings as required type
	{
		pathSettings->setSourcePaths(m_list->getPathsAsDisplayed());
	}
}

std::vector<FilePath> QtProjectWizardContentPathsSource::getFilePaths() const
{
	std::set<FilePath> allSourceFilePaths;

#if BUILD_CXX_LANGUAGE_PACKAGE
	if (std::dynamic_pointer_cast<SourceGroupSettingsWithCxxPathsAndFlags>(m_settings))
	{
		allSourceFilePaths = SourceGroupCxxEmpty(m_settings).getAllSourceFilePaths();
	}
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
	if (std::shared_ptr<SourceGroupSettingsJavaEmpty> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsJavaEmpty>(m_settings))
	{
		allSourceFilePaths = SourceGroupJavaEmpty(settings).getAllSourceFilePaths();
	}
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

#if BUILD_PYTHON_LANGUAGE_PACKAGE
	if (std::shared_ptr<SourceGroupSettingsPythonEmpty> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsPythonEmpty>(m_settings))
	{
		allSourceFilePaths = SourceGroupPythonEmpty(settings).getAllSourceFilePaths();
	}
#endif	  // BUILD_PYTHON_LANGUAGE_PACKAGE

	if (std::shared_ptr<SourceGroupSettingsCustomCommand> settings =
			std::dynamic_pointer_cast<SourceGroupSettingsCustomCommand>(m_settings))
	{
		allSourceFilePaths = SourceGroupCustomCommand(settings).getAllSourceFilePaths();
	}

	return utility::getAsRelativeIfShorter(
		utility::toVector(allSourceFilePaths), m_settings->getProjectDirectoryPath());
}

QString QtProjectWizardContentPathsSource::getFileNamesTitle() const
{
	return QStringLiteral("Indexed Files");
}

QString QtProjectWizardContentPathsSource::getFileNamesDescription() const
{
	return QStringLiteral(" files will be indexed.");
}
