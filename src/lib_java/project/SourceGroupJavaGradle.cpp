#include "project/SourceGroupJavaGradle.h"

#include "component/view/DialogView.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ScopedFunctor.h"
#include "utility/utility.h"
#include "utility/utilityGradle.h"
#include "Application.h"

SourceGroupJavaGradle::SourceGroupJavaGradle(std::shared_ptr<SourceGroupSettingsJavaGradle> settings)
	: m_settings(settings)
{
}

SourceGroupJavaGradle::~SourceGroupJavaGradle()
{
}

SourceGroupType SourceGroupJavaGradle::getType() const
{
	return SOURCE_GROUP_JAVA_GRADLE;
}

bool SourceGroupJavaGradle::prepareIndexing()
{
	if (!SourceGroupJava::prepareIndexing())
	{
		return false;
	}

	if (!prepareGradleData())
	{
		return false;
	}

	return true;
}

std::vector<FilePath> SourceGroupJavaGradle::doGetClassPath()
{
	std::vector<FilePath> classPath = SourceGroupJava::doGetClassPath();

	if (m_settings->getGradleDependenciesDirectoryExpandedAndAbsolute().exists())
	{
		std::vector<FilePath> gradleJarPaths = FileSystem::getFilePathsFromDirectory(
			m_settings->getGradleDependenciesDirectoryExpandedAndAbsolute(),
			{".jar"}
		);

		for (const FilePath& gradleJarPath : gradleJarPaths)
		{
			LOG_INFO("Adding jar to classpath: " + gradleJarPath.str());
		}

		utility::append(classPath, gradleJarPaths);
	}

	return classPath;
}

std::shared_ptr<SourceGroupSettingsJava> SourceGroupJavaGradle::getSourceGroupSettingsJava()
{
	return m_settings;
}

std::vector<FilePath> SourceGroupJavaGradle::getAllSourcePaths() const
{
	std::vector<FilePath> sourcePaths;
	if (m_settings->getGradleProjectFilePathExpandedAndAbsolute().exists())
	{
		std::shared_ptr<DialogView> dialogView = Application::getInstance()->getDialogView();
		dialogView->showUnknownProgressDialog("Preparing Project", "Gradle\nFetching Source Directories");

		const FilePath projectRootPath = m_settings->getGradleProjectFilePathExpandedAndAbsolute().getParentDirectory();
		sourcePaths = utility::gradleGetAllSourceDirectories(projectRootPath, m_settings->getShouldIndexGradleTests());

		dialogView->hideUnknownProgressDialog();
	}
	return sourcePaths;
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
		
		dialogView->showUnknownProgressDialog("Preparing Project", "Gradle\nExporting Dependencies");

		bool success = utility::gradleCopyDependencies(
			projectRootPath, 
			m_settings->getGradleDependenciesDirectoryExpandedAndAbsolute(), 
			m_settings->getShouldIndexGradleTests()
		);

		return success;
	}

	return true;
}
