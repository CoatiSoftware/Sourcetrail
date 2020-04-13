#include "SourceGroupJavaMaven.h"

#include "../../lib/app/Application.h"
#include "../../lib/settings/ApplicationSettings.h"
#include "../../lib/component/view/DialogView.h"
#include "../../lib/utility/file/FileSystem.h"
#include "../../lib/utility/messaging/type/MessageStatus.h"
#include "../../lib/utility/ScopedFunctor.h"
#include "../../lib/settings/source_group/type/SourceGroupSettingsJavaMaven.h"
#include "../../lib/utility/logging/logging.h"
#include "../../lib/utility/utility.h"
#include "../utility/utilityJava.h"
#include "../utility/utilityMaven.h"

SourceGroupJavaMaven::SourceGroupJavaMaven(std::shared_ptr<SourceGroupSettingsJavaMaven> settings)
	: m_settings(settings)
	, m_allSourcePathsCache(std::bind(&SourceGroupJavaMaven::doGetAllSourcePaths, this))
{
}

bool SourceGroupJavaMaven::prepareIndexing()
{
	if (!utility::prepareJavaEnvironmentAndDisplayOccurringErrors())
	{
		return false;
	}

	if (!prepareMavenData())
	{
		return false;
	}

	return true;
}

std::vector<FilePath> SourceGroupJavaMaven::getAllSourcePaths() const
{
	return m_allSourcePathsCache.getValue();
}

std::vector<FilePath> SourceGroupJavaMaven::doGetClassPath() const
{
	std::vector<FilePath> classPath = utility::getClassPath({}, true, getAllSourceFilePaths());

	if (m_settings && m_settings->getMavenDependenciesDirectoryPath().exists())
	{
		std::vector<FilePath> mavenJarPaths = FileSystem::getFilePathsFromDirectory(
			m_settings->getMavenDependenciesDirectoryPath(), {L".jar"});

		for (const FilePath& mavenJarPath: mavenJarPaths)
		{
			LOG_INFO(L"Adding jar to classpath: " + mavenJarPath.wstr());
		}

		utility::append(classPath, mavenJarPaths);
	}

	return classPath;
}

std::shared_ptr<SourceGroupSettings> SourceGroupJavaMaven::getSourceGroupSettings()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettings> SourceGroupJavaMaven::getSourceGroupSettings() const
{
	return m_settings;
}

bool SourceGroupJavaMaven::prepareMavenData()
{
	if (m_settings && m_settings->getMavenProjectFilePathExpandedAndAbsolute().exists())
	{
		const FilePath mavenPath = ApplicationSettings::getInstance()->getMavenPath();
		const FilePath mavenSettingsPath = m_settings->getMavenSettingsFilePathExpandedAndAbsolute();
		const FilePath projectRootPath =
			m_settings->getMavenProjectFilePathExpandedAndAbsolute().getParentDirectory();

		std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView(
			DialogView::UseCase::PROJECT_SETUP);
		dialogView->showUnknownProgressDialog(
			L"Preparing Project", L"Maven\nGenerating Source Files");

		ScopedFunctor dialogHider([&dialogView]() { dialogView->hideUnknownProgressDialog(); });

		const std::wstring errorMessage = utility::mavenGenerateSources(
			mavenPath, mavenSettingsPath, projectRootPath);
		if (!errorMessage.empty())
		{
			MessageStatus(errorMessage, true, false).dispatch();
			Application::getInstance()->handleDialog(errorMessage);
			return false;
		}

		dialogView->showUnknownProgressDialog(
			L"Preparing Project", L"Maven\nExporting Dependencies");

		bool success = utility::mavenCopyDependencies(
			mavenPath,
			mavenSettingsPath,
			projectRootPath,
			m_settings->getMavenDependenciesDirectoryPath());

		return success;
	}

	return true;
}

std::vector<FilePath> SourceGroupJavaMaven::doGetAllSourcePaths() const
{
	std::vector<FilePath> sourcePaths;
	if (m_settings && m_settings->getMavenProjectFilePathExpandedAndAbsolute().exists())
	{
		std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView(
			DialogView::UseCase::PROJECT_SETUP);
		dialogView->showUnknownProgressDialog(
			L"Preparing Project", L"Maven\nFetching Source Directories");

		const FilePath mavenPath(ApplicationSettings::getInstance()->getMavenPath());
		const FilePath mavenSettingsPath = m_settings->getMavenSettingsFilePathExpandedAndAbsolute();
		const FilePath projectRootPath =
			m_settings->getMavenProjectFilePathExpandedAndAbsolute().getParentDirectory();

		sourcePaths = utility::mavenGetAllDirectoriesFromEffectivePom(
			mavenPath,
			mavenSettingsPath,
			projectRootPath,
			m_settings->getMavenDependenciesDirectoryPath(),
			m_settings->getShouldIndexMavenTests());

		dialogView->hideUnknownProgressDialog();
	}
	return sourcePaths;
}
