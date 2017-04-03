#include "utility/utilityMaven.h"

#include "utility/text/TextAccess.h"
#include "utility/utilityApp.h"
#include "utility/utilityString.h"
#include "utility/utilityXml.h"
#include "utility/utility.h"

namespace utility
{
	bool mavenGenerateSources(const FilePath& mavenPath, const FilePath& projectDirectoryPath)
	{
		const std::string output = utility::executeProcess(
			"\"" + mavenPath.str() + "\" generate-sources",
			projectDirectoryPath.str()
		);
		return !output.empty();
	}

	bool mavenCopyDependencies(const FilePath& mavenPath, const FilePath& projectDirectoryPath, const FilePath& outputDirectoryPath)
	{
		const std::string output = utility::executeProcess(
			"\"" + mavenPath.str() + "\" dependency:copy-dependencies -DoutputDirectory=" + outputDirectoryPath.str(),
			projectDirectoryPath.str()
		);
		return !output.empty();
	}

	std::vector<FilePath> mavenGetAllDirectoriesFromEffectivePom(const FilePath& mavenPath, const FilePath& projectDirectoryPath, bool addTestDirectories)
	{

		std::shared_ptr<TextAccess> outputAccess = TextAccess::createFromString(utility::executeProcess(
			"\"" + mavenPath.str() + "\" help:effective-pom",
			projectDirectoryPath.str()
		));

		std::string xmlContent = "";
		for (std::string line: outputAccess->getAllLines())
		{
			const std::string trimmedLine = utility::trim(line);
			if (!trimmedLine.empty() && trimmedLine.front() == '<')
			{
				xmlContent.append(line);
			}
		}
		std::shared_ptr<TextAccess> xmlAccess = TextAccess::createFromString(xmlContent);

		std::vector<FilePath> directories;


		std::vector<FilePath> uncheckedDirectories;
		for (std::string value: utility::getValuesOfAllXmlElementsOnPath(xmlAccess, utility::createVectorFromElements<std::string>("project", "build", "sourceDirectory")))
		{
			uncheckedDirectories.push_back(FilePath(value));
		}

		for (std::string value: utility::getValuesOfAllXmlElementsOnPath(xmlAccess, utility::createVectorFromElements<std::string>("projects", "project", "build", "sourceDirectory")))
		{
			uncheckedDirectories.push_back(FilePath(value));
		}

		for (std::string value: utility::getValuesOfAllXmlElementsOnPath(xmlAccess, utility::createVectorFromElements<std::string>("project", "build", "directory")))
		{
			uncheckedDirectories.push_back(FilePath(value).concat("generated-sources"));
		}

		for (std::string value: utility::getValuesOfAllXmlElementsOnPath(xmlAccess, utility::createVectorFromElements<std::string>("projects", "project", "build", "directory")))
		{
			uncheckedDirectories.push_back(FilePath(value).concat("generated-sources"));
		}

		if (addTestDirectories)
		{
			for (std::string value: utility::getValuesOfAllXmlElementsOnPath(xmlAccess, utility::createVectorFromElements<std::string>("project", "build", "testSourceDirectory")))
			{
				uncheckedDirectories.push_back(FilePath(value));
			}
			for (std::string value: utility::getValuesOfAllXmlElementsOnPath(xmlAccess, utility::createVectorFromElements<std::string>("projects", "project", "build", "testSourceDirectory")))
			{
				uncheckedDirectories.push_back(FilePath(value));
			}

			for (std::string value: utility::getValuesOfAllXmlElementsOnPath(xmlAccess, utility::createVectorFromElements<std::string>("project", "build", "directory")))
			{
				uncheckedDirectories.push_back(FilePath(value).concat("generated-test-sources"));
			}

			for (std::string value: utility::getValuesOfAllXmlElementsOnPath(xmlAccess, utility::createVectorFromElements<std::string>("projects", "project", "build", "directory")))
			{
				uncheckedDirectories.push_back(FilePath(value).concat("generated-test-sources"));
			}
		}

		for (const FilePath& uncheckedDirectory: uncheckedDirectories)
		{
			if (uncheckedDirectory.exists())
			{
				directories.push_back(uncheckedDirectory);
			}
		}

		return directories;
	}
}
