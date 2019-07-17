#include "SourceGroupJavaGradle.h"

#include "Application.h"
#include "DialogView.h"
#include "FileSystem.h"
#include "logging.h"
#include "SourceGroupSettingsJavaGradle.h"
#include "ScopedFunctor.h"
#include "utility.h"
#include "utilityGradle.h"
#include "utilityJava.h"

SourceGroupJavaGradle::SourceGroupJavaGradle(std::shared_ptr<SourceGroupSettingsJavaGradle> settings)
	: m_settings(settings)
{
}

bool SourceGroupJavaGradle::prepareIndexing()
{
	if (!utility::prepareJavaEnvironmentAndDisplayOccurringErrors())
	{
		return false;
	}

	if (!prepareGradleData())
	{
		return false;
	}

	return true;
}

std::vector<FilePath> SourceGroupJavaGradle::getAllSourcePaths() const
{
	std::vector<FilePath> sourcePaths;
	if (m_settings->getGradleProjectFilePathExpandedAndAbsolute().exists())
	{
		std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView(DialogView::UseCase::PROJECT_SETUP);
		dialogView->showUnknownProgressDialog(L"Preparing Project", L"Gradle\nFetching Source Directories");

		const FilePath projectRootPath = m_settings->getGradleProjectFilePathExpandedAndAbsolute().getParentDirectory();
		sourcePaths = utility::gradleGetAllSourceDirectories(projectRootPath, m_settings->getShouldIndexGradleTests());

		dialogView->hideUnknownProgressDialog();
	}
	else
	{
		LOG_INFO("Could not find any source paths because Gradle project path does not exist.");
	}
	return sourcePaths;
}

std::vector<FilePath> SourceGroupJavaGradle::doGetClassPath() const
{
	std::vector<FilePath> classPath = utility::getClassPath({}, true, getAllSourceFilePaths());

	if (m_settings->getGradleDependenciesDirectoryPath().exists())
	{
		std::vector<FilePath> gradleJarPaths = FileSystem::getFilePathsFromDirectory(
			m_settings->getGradleDependenciesDirectoryPath(),
			{ L".jar" }
		);

		for (const FilePath& gradleJarPath : gradleJarPaths)
		{
			LOG_INFO(L"Adding jar to classpath: " + gradleJarPath.wstr());
		}

		utility::append(classPath, gradleJarPaths);
	}

	return classPath;
}

std::shared_ptr<SourceGroupSettingsJava> SourceGroupJavaGradle::getSourceGroupSettingsJava()
{
	return m_settings;
}

std::shared_ptr<const SourceGroupSettingsJava> SourceGroupJavaGradle::getSourceGroupSettingsJava() const
{
	return m_settings;
}

bool SourceGroupJavaGradle::prepareGradleData()
{
	if (m_settings && m_settings->getGradleProjectFilePathExpandedAndAbsolute().exists())
	{
		const FilePath projectRootPath = m_settings->getGradleProjectFilePathExpandedAndAbsolute().getParentDirectory();

		std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView(DialogView::UseCase::PROJECT_SETUP);

		ScopedFunctor dialogHider([&dialogView]() {
			dialogView->hideUnknownProgressDialog();
		});

		dialogView->showUnknownProgressDialog(L"Preparing Project", L"Gradle\nExporting Dependencies");

		bool success = utility::gradleCopyDependencies(
			projectRootPath,
			m_settings->getGradleDependenciesDirectoryPath(),
			m_settings->getShouldIndexGradleTests()
		);

		return success;
	}

	return true;
}
