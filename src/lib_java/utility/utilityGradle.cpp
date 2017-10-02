#include "utility/utilityGradle.h"

#include <set>

#include "data/parser/java/JavaEnvironment.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "settings/ApplicationSettings.h"
#include "utility/logging/logging.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityJava.h"
#include "utility/utilityString.h"

namespace
{
	void setJavaHomeVariableIfNotExists()
	{
		if (getenv("JAVA_HOME") == nullptr)
		{
			const FilePath javaPath(ApplicationSettings::getInstance()->getJavaPath());
			const FilePath javaHomePath = javaPath.parentDirectory().parentDirectory().parentDirectory();

			LOG_WARNING("Environment variable \"JAVA_HOME\" not found on system. Setting value to \"" + javaHomePath.str() + "\" for this process.");

			putenv(const_cast<char*>(("JAVA_HOME=" + javaHomePath.str()).c_str()));
		}
	}
}

namespace utility
{
	bool gradleCopyDependencies(const FilePath& projectDirectoryPath, const FilePath& outputDirectoryPath, bool addTestDependencies)
	{
		const std::string gradleInitScriptPath = ResourcePaths::getJavaPath().str() + "gradle/init.gradle";

		setJavaHomeVariableIfNotExists();
		utility::prepareJavaEnvironment();

		std::shared_ptr<JavaEnvironment> javaEnvironment = JavaEnvironmentFactory::getInstance()->createEnvironment();
		bool success = javaEnvironment->callStaticVoidMethod("com/sourcetrail/gradle/InfoRetriever", "copyCompileLibs", projectDirectoryPath.str(), gradleInitScriptPath, outputDirectoryPath.str());
		
		if (success && addTestDependencies)
		{
			success = javaEnvironment->callStaticVoidMethod("com/sourcetrail/gradle/InfoRetriever", "copyTestCompileLibs", projectDirectoryPath.str(), gradleInitScriptPath, outputDirectoryPath.str());
		}

		return success;
	}

	std::vector<FilePath> gradleGetAllSourceDirectories(const FilePath& projectDirectoryPath, bool addTestDirectories)
	{
		const std::string gradleInitScriptPath = ResourcePaths::getJavaPath().str() + "gradle/init.gradle";

		setJavaHomeVariableIfNotExists();
		utility::prepareJavaEnvironment();

		std::set<std::string> uncheckedDirectories;
		{
			std::shared_ptr<JavaEnvironment> javaEnvironment = JavaEnvironmentFactory::getInstance()->createEnvironment();
			{
				std::string mainSrcDirs = "";
				javaEnvironment->callStaticStringMethod("com/sourcetrail/gradle/InfoRetriever", "getMainSrcDirs", mainSrcDirs, projectDirectoryPath.str(), gradleInitScriptPath);
				for (const std::string mainSrcDir : utility::splitToVector(mainSrcDirs, ";"))
				{
					uncheckedDirectories.insert(mainSrcDir);
				}
			}

			if (addTestDirectories)
			{
				std::string testSrcDirs = "";
				javaEnvironment->callStaticStringMethod("com/sourcetrail/gradle/InfoRetriever", "getTestSrcDirs", testSrcDirs, projectDirectoryPath.str(), gradleInitScriptPath);
				for (const std::string testSrcDir : utility::splitToVector(testSrcDirs, ";"))
				{
					uncheckedDirectories.insert(testSrcDir);
				}
			}
		}

		std::vector<FilePath> directories;
		for (const std::string& uncheckedDirectory: uncheckedDirectories)
		{
			FilePath uncheckedDirectoryPath(uncheckedDirectory);
			if (uncheckedDirectoryPath.exists())
			{
				directories.push_back(uncheckedDirectoryPath);
			}
		}

		LOG_INFO("Found " + std::to_string(directories.size()) + " of " + std::to_string(uncheckedDirectories.size()) + " directories on system.");

		return directories;
	}
}
