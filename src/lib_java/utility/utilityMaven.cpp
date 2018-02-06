#include "utility/utilityMaven.h"

#include <cstdlib>

#include "settings/ApplicationSettings.h"
#include "utility/file/FilePath.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityApp.h"
#include "utility/utilityString.h"
#include "utility/utilityXml.h"
#include "utility/utility.h"
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
}

namespace utility
{
	bool mavenGenerateSources(const FilePath& mavenPath, const FilePath& projectDirectoryPath)
	{
		setJavaHomeVariableIfNotExists();

		const std::string output = utility::executeProcessUntilNoOutput(
			"\"" + mavenPath.str() + "\" generate-sources",
			projectDirectoryPath.str(),
			60000
		);
		return !output.empty();
	}

	bool mavenCopyDependencies(const FilePath& mavenPath, const FilePath& projectDirectoryPath, const FilePath& outputDirectoryPath)
	{
		setJavaHomeVariableIfNotExists();

		const std::string output = utility::executeProcessUntilNoOutput(
			"\"" + mavenPath.str() + "\" dependency:copy-dependencies -DoutputDirectory=" + outputDirectoryPath.str(),
			projectDirectoryPath.str(),
			60000
		);

		std::shared_ptr<TextAccess> outputAccess = TextAccess::createFromString(output);
		for (const std::string& line: outputAccess->getAllLines())
		{
			if (utility::isPrefix("[ERROR]", utility::trim(line)))
			{
				// TODO: move error handling to caller of this function
				const std::string dialogMessage =
					"The following error occurred while executing a Maven command:\n\n" + utility::replace(line, "\r\n", "\n");
				MessageStatus(dialogMessage, true, false).dispatch();
				Application::getInstance()->handleDialog(dialogMessage);
				return false;
			}
		}

		return !output.empty();
	}

	std::vector<FilePath> mavenGetAllDirectoriesFromEffectivePom(const FilePath& mavenPath, const FilePath& projectDirectoryPath, bool addTestDirectories)
	{
		setJavaHomeVariableIfNotExists();

		std::shared_ptr<TextAccess> outputAccess = TextAccess::createFromString(utility::executeProcessUntilNoOutput(
			"\"" + mavenPath.str() + "\" help:effective-pom",
			projectDirectoryPath.str(),
			60000
		));

		if (outputAccess->getLineCount() > 0 && utility::isPrefix("Error", utility::trim(outputAccess->getLine(1))))
		{
			// TODO: move error handling to caller of this function
			const std::string dialogMessage =
				"The following error occurred while executing a Maven command:\n\n" + utility::replace(outputAccess->getText(), "\r\n", "\n");

			MessageStatus(dialogMessage, true, false).dispatch();

			Application::getInstance()->handleDialog(dialogMessage);

			return std::vector<FilePath>();
		}

		size_t startLine = 0;
		for (size_t i = 1; i <= outputAccess->getLineCount(); i++)
		{
			if (utility::isPrefix("<", utility::trim(outputAccess->getLine(i))))
			{
				startLine = i;
				break;
			}
		}

		size_t endLine = outputAccess->getLineCount();
		for (size_t i = outputAccess->getLineCount(); i > 0 ; i--)
		{
			if (utility::isPrefix("<", utility::trim(outputAccess->getLine(i))))
			{
				endLine = i;
				break;
			}
		}
		for (size_t i = endLine + 1; i <= outputAccess->getLineCount(); i++)
		{
			if (utility::isPrefix("[", utility::trim(outputAccess->getLine(i))))
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
