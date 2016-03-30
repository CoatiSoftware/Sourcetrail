#include "SolutionParserCodeBlocks.h"

#include <set>

#include "SolutionParserUtility.h"

#include "utility/logging/logging.h"

SolutionParserCodeBlocks::SolutionParserCodeBlocks()
{

}

SolutionParserCodeBlocks::~SolutionParserCodeBlocks()
{

}

std::string SolutionParserCodeBlocks::getToolID() const
{
	return "cb";
}

std::string SolutionParserCodeBlocks::getSolutionName()
{
	std::string result = "";

	if (m_solutionName.size() > 0)
	{
		size_t pos = m_solutionName.find(".cbp");

		if (pos != std::string::npos)
		{
			result = m_solutionName.substr(0, pos);
		}
		else
		{
			result = m_solution;
		}
	}

	return result;
}

std::vector<std::string> SolutionParserCodeBlocks::getProjects()
{
	std::vector<std::string> projectFiles;

	projectFiles.push_back(m_solutionName);

	return projectFiles;
}

std::vector<std::string> SolutionParserCodeBlocks::getProjectFiles()
{
	std::vector<std::string> projectFiles;
	std::vector<std::string> projectFilesNames = getProjects();

	projectFiles.push_back(loadFile(m_solutionPath + m_solutionName));

	return projectFiles;
}

std::vector<std::string> SolutionParserCodeBlocks::getProjectItems()
{
	std::vector<std::string> projectItems;

	std::vector<std::string> projectFilesNames = getProjects();
	std::vector<std::string> projectFiles = getProjectFiles();

	std::vector<std::string> validFileExtensions;
	validFileExtensions.push_back(".c");
	validFileExtensions.push_back(".cpp");
	validFileExtensions.push_back(".h");
	validFileExtensions.push_back(".hpp");

	for (unsigned int i = 0; i < projectFiles.size(); i++)
	{
		TiXmlDocument doc;
		doc.Parse(projectFiles[i].c_str(), 0, TIXML_ENCODING_UTF8);

		std::string error = doc.ErrorDesc();

		if (error.length() > 0)
		{
			LOG_ERROR_STREAM(<< "Failed to parse project file " << i << ": " << error);
			continue;
		}

		std::vector<TiXmlElement*> nodes = SolutionParserUtility::getAllTagsByNameWithAttribute(doc.RootElement(), "Unit", "filename");

		if (nodes.size() > 0)
		{
			for (unsigned int i = 0; i < nodes.size(); i++)
			{
				std::string text = nodes[i]->Attribute("filename");

				if (SolutionParserUtility::checkValidFileExtension(text, validFileExtensions))
				{
					if (boost::filesystem::exists(text))
					{
						projectItems.push_back(text);
						continue;
					}

					text = m_solutionPath + "/" + text;

					projectItems.push_back(text);
				}
			}
		}
	}

	std::set<std::string> s(projectItems.begin(), projectItems.end());
	projectItems.assign(s.begin(), s.end());

	projectItems = SolutionParserUtility::resolveEnvironmentVariables(projectItems);
	projectItems = makePathsAbsolute(projectItems);

	return projectItems;
}

std::vector<std::string> SolutionParserCodeBlocks::getIncludePaths()
{
	std::vector<std::string> includePaths;

	std::vector<std::string> projectFiles = getProjectFiles();

	std::vector<std::string> validExtensions;
	validExtensions.push_back(".c");
	validExtensions.push_back(".cpp");
	validExtensions.push_back(".h");
	validExtensions.push_back(".hpp");

	for (unsigned int i = 0; i < projectFiles.size(); i++)
	{
		TiXmlDocument doc;
		doc.Parse(projectFiles[i].c_str(), 0, TIXML_ENCODING_UTF8);

		std::string error = doc.ErrorDesc();

		if (error.length() > 0)
		{
			LOG_ERROR_STREAM(<< "Failed to parse project file " << i << ": " << error);
			continue;
		}

		std::vector<TiXmlElement*> nodes = SolutionParserUtility::getAllTagsByNameWithAttribute(doc.RootElement(), "Add", "directory");

		if (nodes.size() > 0)
		{
			for (unsigned int i = 0; i < nodes.size(); i++)
			{
				std::string text = nodes[i]->Attribute("directory");

				includePaths.push_back(text);
			}
		}
	}

	std::set<std::string> s(includePaths.begin(), includePaths.end());
	includePaths.assign(s.begin(), s.end());

	includePaths = SolutionParserUtility::resolveEnvironmentVariables(includePaths);
	includePaths = makePathsAbsolute(includePaths);

	return includePaths;
}

std::vector<std::string> SolutionParserCodeBlocks::getCompileFlags()
{
	std::vector<std::string> compilerFlags;

	return compilerFlags;
}

ProjectSettings SolutionParserCodeBlocks::getProjectSettings(const std::string& solutionFilePath)
{
	openSolutionFile(solutionFilePath);

	ProjectSettings settings;
	settings.setProjectName(getSolutionName());
	settings.setProjectFileLocation(getSolutionPath());
	settings.setVisualStudioSolutionPath(solutionFilePath);

	std::vector<std::string> sourceFiles = getProjectItems();
	std::vector<FilePath> sourcePaths;
	for (const std::string& p : sourceFiles)
	{
		sourcePaths.push_back(FilePath(p));
	}

	std::vector<std::string> includePaths = getIncludePaths();
	std::vector<FilePath> headerPaths;
	for (const std::string& p : includePaths)
	{
		headerPaths.push_back(FilePath(p));
	}

	settings.setSourcePaths(sourcePaths);
	settings.setHeaderSearchPaths(headerPaths);

	return settings;
}

std::string SolutionParserCodeBlocks::getIdeName() const
{
	return "Code Blocks";
}

std::string SolutionParserCodeBlocks::getDescription() const
{
	return "idunno";
}

std::string SolutionParserCodeBlocks::getIconPath() const
{
	return "icon/project_vs_256_256.png";
}

std::string SolutionParserCodeBlocks::getFileExtension() const
{
	return ".cbp";
}
