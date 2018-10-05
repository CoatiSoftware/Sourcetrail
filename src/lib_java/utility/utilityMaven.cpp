#include "utilityMaven.h"

#include <cstdlib>

#include "ApplicationSettings.h"
#include "FilePath.h"
#include "MessageStatus.h"
#include "TextAccess.h"
#include "utilityApp.h"
#include "utilityString.h"
#include "utilityXml.h"
#include "utility.h"
#include "Application.h"

namespace
{
	void fetchDirectories(std::vector<FilePath>& pathList, std::shared_ptr<TextAccess> xmlAccess, const std::vector<std::string>& tags, const FilePath& toAppend = FilePath())
	{
		{
			std::string tagString = "";
			for (size_t i = 0; i < tags.size(); i++)
			{
				if (i != 0)
				{
					tagString += " -> ";
				}
				tagString += tags[i];
			}
			LOG_INFO("Fetching source directories in \"" + tagString + "\".");
		}

		std::vector<std::string> fetchedDirectories = utility::getValuesOfAllXmlElementsOnPath(
			xmlAccess, tags
		);
		LOG_INFO("Found " + std::to_string(fetchedDirectories.size()) + " source directories.");

		for (const std::string& fetchedDirectory: fetchedDirectories)
		{
			FilePath path(fetchedDirectory);
			if (!toAppend.empty())
			{
				path.concatenate(toAppend);
			}
			pathList.push_back(path);
			LOG_INFO(L"Found directory \"" + path.wstr() + L"\".");
		}
	}

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

	std::wstring getErrorMessageFromMavenOutput(std::shared_ptr<const TextAccess> mavenOutput)
	{
		const std::string errorPrefix = "[ERROR]";
		const std::string fatalPrefix = "[FATAL]";

		std::wstring errorMessage;

		for (const std::string& line : mavenOutput->getAllLines())
		{
			const std::string trimmedLine = utility::trim(line);

			if (utility::isPrefix<std::string>(errorPrefix, trimmedLine))
			{
				errorMessage += utility::decodeFromUtf8(utility::trim(trimmedLine.substr(errorPrefix.size())) + "\n");
			}
			else if (utility::isPrefix<std::string>(fatalPrefix, trimmedLine))
			{
				errorMessage += utility::decodeFromUtf8(trimmedLine + "\n");
			}
		}

		if (!errorMessage.empty())
		{
			errorMessage = L"The following error occurred while executing a Maven command:\n\n" + errorMessage;
		}

		return errorMessage;
	}
}

namespace utility
{
	std::wstring mavenGenerateSources(const FilePath& mavenPath, const FilePath& projectDirectoryPath)
	{
		setJavaHomeVariableIfNotExists();

		std::shared_ptr<TextAccess> outputAccess = TextAccess::createFromString(utility::executeProcessUntilNoOutput(
			"\"" + mavenPath.str() + "\" generate-sources",
			projectDirectoryPath,
			60000
		));

		if (outputAccess->isEmpty())
		{
			return	L"Sourcetrail was unable to locate Maven on this machine.\nPlease make sure to provide the correct Maven Path in the preferences.";
		}

		return getErrorMessageFromMavenOutput(outputAccess);
	}

	bool mavenCopyDependencies(const FilePath& mavenPath, const FilePath& projectDirectoryPath, const FilePath& outputDirectoryPath)
	{
		setJavaHomeVariableIfNotExists();

		std::shared_ptr<TextAccess> outputAccess = TextAccess::createFromString(utility::executeProcessUntilNoOutput(
			"\"" + mavenPath.str() + "\" dependency:copy-dependencies -DoutputDirectory=" + outputDirectoryPath.str(),
			projectDirectoryPath,
			60000
		));

		const std::wstring errorMessage = getErrorMessageFromMavenOutput(outputAccess);
		if (!errorMessage.empty())
		{
			MessageStatus(errorMessage, true, false).dispatch();
			Application::getInstance()->handleDialog(errorMessage);
			return false;
		}

		return !outputAccess->isEmpty();
	}

	std::vector<FilePath> mavenGetAllDirectoriesFromEffectivePom(const FilePath& mavenPath, const FilePath& projectDirectoryPath, bool addTestDirectories)
	{
		setJavaHomeVariableIfNotExists();

		std::shared_ptr<TextAccess> outputAccess = TextAccess::createFromString(utility::executeProcessUntilNoOutput(
			"\"" + mavenPath.str() + "\" help:effective-pom",
			projectDirectoryPath,
			60000
		));

		const std::wstring errorMessage = getErrorMessageFromMavenOutput(outputAccess);
		if (!errorMessage.empty())
		{
			MessageStatus(errorMessage, true, false).dispatch();
			Application::getInstance()->handleDialog(errorMessage);
			return std::vector<FilePath>();
		}

		size_t startLine = 0;
		for (size_t i = 1; i <= outputAccess->getLineCount(); i++)
		{
			if (utility::isPrefix<std::string>("<", utility::trim(outputAccess->getLine(i))))
			{
				startLine = i;
				break;
			}
		}

		size_t endLine = outputAccess->getLineCount();
		for (size_t i = outputAccess->getLineCount(); i > 0 ; i--)
		{
			if (utility::isPrefix<std::string>("<", utility::trim(outputAccess->getLine(i))))
			{
				endLine = i;
				break;
			}
		}
		for (size_t i = endLine + 1; i <= outputAccess->getLineCount(); i++)
		{
			if (utility::isPrefix<std::string>("[", utility::trim(outputAccess->getLine(i))))
			{
				break;
			}
			endLine = i;
		}

		std::string xmlContent = "";
		for (const std::string& line: outputAccess->getLines(startLine, endLine))
		{
			xmlContent.append(line);
		}
		std::shared_ptr<TextAccess> xmlAccess = TextAccess::createFromString(xmlContent);

		std::vector<FilePath> uncheckedDirectories;
		fetchDirectories(uncheckedDirectories, xmlAccess,
			utility::createVectorFromElements<std::string>("project", "build", "sourceDirectory"));
		fetchDirectories(uncheckedDirectories, xmlAccess,
			utility::createVectorFromElements<std::string>("projects", "project", "build", "sourceDirectory"));
		fetchDirectories(uncheckedDirectories, xmlAccess,
			utility::createVectorFromElements<std::string>("project", "build", "directory"), FilePath(L"generated-sources"));
		fetchDirectories(uncheckedDirectories, xmlAccess,
			utility::createVectorFromElements<std::string>("projects", "project", "build", "directory"), FilePath(L"generated-sources"));

		if (addTestDirectories)
		{
			fetchDirectories(uncheckedDirectories, xmlAccess,
				utility::createVectorFromElements<std::string>("project", "build", "testSourceDirectory"));
			fetchDirectories(uncheckedDirectories, xmlAccess,
				utility::createVectorFromElements<std::string>("projects", "project", "build", "testSourceDirectory"));
			fetchDirectories(uncheckedDirectories, xmlAccess,
				utility::createVectorFromElements<std::string>("project", "build", "directory"), FilePath(L"generated-test-sources"));
			fetchDirectories(uncheckedDirectories, xmlAccess,
				utility::createVectorFromElements<std::string>("projects", "project", "build", "directory"), FilePath(L"generated-test-sources"));
		}

		std::vector<FilePath> directories;
		for (const FilePath& uncheckedDirectory: uncheckedDirectories)
		{
			if (uncheckedDirectory.exists())
			{
				directories.push_back(uncheckedDirectory);
			}
		}

		LOG_INFO("Found " + std::to_string(directories.size()) + " of " + std::to_string(uncheckedDirectories.size()) + " directories on system.");

		return directories;
	}
}
