#include "project/SourceGroupJavaGradle.h"

#include "component/view/DialogView.h"
#include "settings/SourceGroupSettingsJavaGradle.h"
#include "utility/file/FileSystem.h"
#include "utility/ScopedFunctor.h"
#include "utility/utility.h"
#include "utility/utilityGradle.h"
#include "utility/utilityJava.h"
#include "Application.h"

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
		std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();
		dialogView->showUnknownProgressDialog(L"Preparing Project", L"Gradle\nFetching Source Directories");

		const FilePath projectRootPath = m_settings->getGradleProjectFilePathExpandedAndAbsolute().getParentDirectory();
		sourcePaths = utility::gradleGetAllSourceDirectories(projectRootPath, m_settings->getShouldIndexGradleTests());

		dialogView->hideUnknownProgressDialog();
	}
	return sourcePaths;
}

std::vector<FilePath> SourceGroupJavaGradle::doGetClassPath() const
{
	std::vector<FilePath> classPath = utility::getClassPath(getSourceGroupSettingsJava(), getAllSourceFilePaths());

	if (m_settings->getGradleDependenciesDirectoryExpandedAndAbsolute().exists())
	{
		std::vector<FilePath> gradleJarPaths = FileSystem::getFilePathsFromDirectory(
			m_settings->getGradleDependenciesDirectoryExpandedAndAbsolute(),
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

		std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();

		ScopedFunctor dialogHider([&dialogView]() {
			dialogView->hideUnknownProgressDialog();
		});

		dialogView->showUnknownProgressDialog(L"Preparing Project", L"Gradle\nExporting Dependencies");

		bool success = utility::gradleCopyDependencies(
			projectRootPath,
			m_settings->getGradleDependenciesDirectoryExpandedAndAbsolute(),
			m_settings->getShouldIndexGradleTests()
		);

		return success;
	}

	return true;
}
