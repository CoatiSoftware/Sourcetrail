#include "utility/utilityGradle.h"

#include <set>

#include "data/parser/java/JavaEnvironment.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "settings/ApplicationSettings.h"
#include "utility/logging/logging.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityJava.h"
#include "utility/utilityString.h"
#include "Application.h"

namespace
{
	void setJavaHomeVariableIfNotExists()
	{
		if (getenv("JAVA_HOME") == nullptr)
		{
			const FilePath javaPath = ApplicationSettings::getInstance()->getJavaPath();
			const FilePath javaHomePath = javaPath.getParentDirectory().getParentDirectory().getParentDirectory();

			LOG_WARNING("Environment variable \"JAVA_HOME\" not found on system. Setting value to \"" + javaHomePath.str() + "\" for this process.");

			putenv(const_cast<char*>(("JAVA_HOME=" + javaHomePath.str()).c_str()));
		}
	}
}

namespace utility
{
	bool gradleCopyDependencies(const FilePath& projectDirectoryPath, const FilePath& outputDirectoryPath, bool addTestDependencies)
	{
		const FilePath gradleInitScriptPath = ResourcePaths::getJavaPath().concatenate(L"gradle/init.gradle");

		setJavaHomeVariableIfNotExists();
		utility::prepareJavaEnvironment();

		std::shared_ptr<JavaEnvironment> javaEnvironment = JavaEnvironmentFactory::getInstance()->createEnvironment();
		bool success = javaEnvironment->callStaticVoidMethod(
			"com/sourcetrail/gradle/InfoRetriever", 
			"copyCompileLibs", 
			utility::encodeToUtf8(projectDirectoryPath.wstr()),
			utility::encodeToUtf8(gradleInitScriptPath.wstr()),
			utility::encodeToUtf8(outputDirectoryPath.wstr())
		);
		
		if (success && addTestDependencies)
		{
			success = javaEnvironment->callStaticVoidMethod(
				"com/sourcetrail/gradle/InfoRetriever",
				"copyTestCompileLibs", 
				utility::encodeToUtf8(projectDirectoryPath.wstr()),
				utility::encodeToUtf8(gradleInitScriptPath.wstr()),
				utility::encodeToUtf8(outputDirectoryPath.wstr())
			);
		}

		return success;
	}

	std::vector<FilePath> gradleGetAllSourceDirectories(const FilePath& projectDirectoryPath, bool addTestDirectories)
	{
		const FilePath gradleInitScriptPath = ResourcePaths::getJavaPath().concatenate(L"gradle/init.gradle");

		setJavaHomeVariableIfNotExists();
		utility::prepareJavaEnvironment();

		std::set<std::wstring> uncheckedDirectories;
		{
			std::shared_ptr<JavaEnvironment> javaEnvironment = JavaEnvironmentFactory::getInstance()->createEnvironment();
			{
				std::string output = "";
				javaEnvironment->callStaticStringMethod(
					"com/sourcetrail/gradle/InfoRetriever", 
					"getMainSrcDirs", 
					output, 
					utility::encodeToUtf8(projectDirectoryPath.wstr()),
					utility::encodeToUtf8(gradleInitScriptPath.wstr())
				);

				if (utility::isPrefix<std::string>("[ERROR]", utility::trim(output)))
				{
					// TODO: move error handling to caller of this function
					const std::wstring dialogMessage =
						L"The following error occurred while executing a Gradle command:\n\n" + 
						utility::decodeFromUtf8(utility::replace(output, "\r\n", "\n"));
					MessageStatus(dialogMessage, true, false).dispatch();
					Application::getInstance()->handleDialog(dialogMessage);

					return std::vector<FilePath>();
				}

				for (const std::string mainSrcDir : utility::splitToVector(output, ";"))
				{
					uncheckedDirectories.insert(utility::decodeFromUtf8(mainSrcDir));
				}
			}

			if (addTestDirectories)
			{
				std::string output = "";
				javaEnvironment->callStaticStringMethod(
					"com/sourcetrail/gradle/InfoRetriever", 
					"getTestSrcDirs", 
					output,
					utility::encodeToUtf8(projectDirectoryPath.wstr()),
					utility::encodeToUtf8(gradleInitScriptPath.wstr())
				);
				
				if (utility::isPrefix<std::string>("[ERROR]", utility::trim(output)))
				{
					// TODO: move error handling to caller of this function
					const std::wstring dialogMessage =
						L"The following error occurred while executing a Gradle command:\n\n" + 
						utility::decodeFromUtf8(utility::replace(output, "\r\n", "\n"));
					MessageStatus(dialogMessage, true, false).dispatch();
					Application::getInstance()->handleDialog(dialogMessage);

					return std::vector<FilePath>();
				}

				for (const std::string testSrcDir : utility::splitToVector(output, ";"))
				{
					uncheckedDirectories.insert(utility::decodeFromUtf8(testSrcDir));
				}
			}
		}

		std::vector<FilePath> directories;
		for (const std::wstring& uncheckedDirectory: uncheckedDirectories)
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
