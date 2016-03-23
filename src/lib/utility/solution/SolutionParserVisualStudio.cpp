#include "SolutionParserVisualStudio.h"

#include <algorithm>
#include <cstdlib>
#include <set>

#include "boost/filesystem/path.hpp"
#include "boost/filesystem.hpp"

#include "SolutionParserUtility.h"

#include "utility/logging/logging.h"

SolutionParserVisualStudio::SolutionParserVisualStudio()
{

}

SolutionParserVisualStudio::~SolutionParserVisualStudio()
{

}

std::string SolutionParserVisualStudio::getToolID() const
{
	return "vs";
}

std::string SolutionParserVisualStudio::getSolutionName()
{
	std::string result = "";

	if (m_solutionName.size() > 0)
	{
		size_t pos = m_solutionName.find(".sln");
		if (pos == std::string::npos)
		{
			pos = m_solutionName.find(".vcxproj");
		}

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

std::vector<std::string> SolutionParserVisualStudio::getProjects()
{
	std::vector<std::string> projectFiles;

	if (m_solutionName.size() > 0 && m_solutionName.find(".vcxproj") != std::string::npos)
	{
		projectFiles.push_back(m_solutionName);
		return projectFiles;
	}

	if (m_solution.size() > 0)
	{
		std::vector<std::string> blocks = getProjectBlocks(m_solution);

		for (unsigned int i = 0; i < blocks.size(); i++)
		{
			projectFiles.push_back(getProjectFilePath(blocks[i]));
		}
	}

	return projectFiles;
}

std::vector<std::string> SolutionParserVisualStudio::getProjectFiles()
{
	std::vector<std::string> projectFiles;
	std::vector<std::string> projectFilesNames = getProjects();

	for (unsigned int i = 0; i < projectFilesNames.size(); i++)
	{
		projectFiles.push_back(loadFile(m_solutionPath + projectFilesNames[i]));
	}

	//projectFiles.push_back(loadFile(m_solutionPath + "foo.xml"));

	return projectFiles;
}

std::vector<std::string> SolutionParserVisualStudio::getProjectItems()
{
	std::vector<std::string> projectItems = findProjectItems();

	projectItems = SolutionParserUtility::makePathsCanonical(projectItems);

	return projectItems;
}

std::vector<std::string> SolutionParserVisualStudio::getIncludePaths()
{
	std::vector<std::string> includePaths = findIncludePaths();

	includePaths = SolutionParserUtility::makePathsCanonical(includePaths);

	return includePaths;
}

ProjectSettings SolutionParserVisualStudio::getProjectSettings(const std::string& solutionFilePath)
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

std::string SolutionParserVisualStudio::getIdeName() const
{
	return "Visual Studio";
}

std::string SolutionParserVisualStudio::getDescription() const
{
	return "Create a new project from an existing Visual Studio Solution file.";
}

std::string SolutionParserVisualStudio::getIconPath() const
{
	return "icon/project_vs_256_256.png";
}

std::string SolutionParserVisualStudio::getFileExtension() const
{
	return ".sln";
}

std::vector<std::string> SolutionParserVisualStudio::getProjectItemsNonCanonical()
{
	return findProjectItems();
}

std::vector<std::string> SolutionParserVisualStudio::getIncludePathsNonCanonical()
{
	return findIncludePaths();
}

std::vector<std::string> SolutionParserVisualStudio::findProjectItems()
{
	std::vector<std::string> projectItems;

	std::vector<std::string> projectFilesNames = getProjects();
	std::vector<std::string> projectFiles = getProjectFiles();

	std::vector<std::string> relativeProjectPaths;

	std::vector<std::string> validFileExtensions;
	validFileExtensions.push_back(".c");
	validFileExtensions.push_back(".cpp");
	validFileExtensions.push_back(".h");
	validFileExtensions.push_back(".hpp");

	for (unsigned int i = 0; i < projectFilesNames.size(); i++)
	{
		size_t pos = projectFilesNames[i].find_last_of("/");
		if (pos == std::string::npos)
		{
			pos = projectFilesNames[i].find_last_of("\\");
		}

		if (pos != std::string::npos)
		{
			relativeProjectPaths.push_back(projectFilesNames[i].substr(0, pos));
		}
		else
		{
			relativeProjectPaths.push_back("");
		}
	}

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

		TiXmlElement* root = SolutionParserUtility::getFirstTagByNameWithAttribute(doc.RootElement(), "ClCompile", "Include");
		TiXmlElement* headerRoot = SolutionParserUtility::getFirstTagByNameWithAttribute(doc.RootElement(), "ClInclude", "Include");
		if (root != NULL)
		{
			// std::string text(root->GetText());
			std::string tag = root->Value();
			std::string text = "";
			if (root->Attribute("Include") != NULL)
			{
				text = root->Attribute("Include");
			}

			if (text.find(".cpp") == std::string::npos && text.find(".c") == std::string::npos)
			{
				root = NULL;
			}
			else
			{
				root = root->Parent()->ToElement();
			}
		}

		if (headerRoot != NULL)
		{
			std::string tag = headerRoot->Value();
			std::string text = "";
			if (headerRoot->Attribute("Include") != NULL)
			{
				text = headerRoot->Attribute("Include");
			}

			if (text.find(".h") == std::string::npos && text.find(".hpp") == std::string::npos)
			{
				headerRoot = NULL;
			}
			else
			{
				headerRoot = headerRoot->Parent()->ToElement();
			}
		}

		/*if (root == NULL)
		{
			TiXmlElement* root = getFirstTagByNameWithAttribute(doc.RootElement(), "ClInclude", "Include");
			if (root != NULL)
			{
				root = root->Parent()->ToElement();
			}
			else
			{
				root = NULL;
			}
		}*/



		if (root != NULL)
		{
			for (TiXmlElement* child = root->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
			{
				std::string filePath = child->Attribute("Include");

				if (boost::filesystem::exists(filePath) && SolutionParserUtility::checkValidFileExtension(filePath, validFileExtensions))
				{
					projectItems.push_back(filePath);
					continue;
				}

				if (relativeProjectPaths[i].size() > 0)
				{
					filePath = relativeProjectPaths[i] + "/" + filePath;
				}

				if (SolutionParserUtility::checkValidFileExtension(filePath, validFileExtensions))
				{
					projectItems.push_back(filePath);
				}
			}
		}

		if (headerRoot != NULL)
		{
			for (TiXmlElement* child = headerRoot->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
			{
				std::string filePath = child->Attribute("Include");

				if (boost::filesystem::exists(filePath) && SolutionParserUtility::checkValidFileExtension(filePath, validFileExtensions))
				{
					projectItems.push_back(filePath);
					continue;
				}

				if (relativeProjectPaths[i].size() > 0)
				{
					filePath = relativeProjectPaths[i] + "/" + filePath;
				}

				if (SolutionParserUtility::checkValidFileExtension(filePath, validFileExtensions))
				{
					projectItems.push_back(filePath);
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

std::vector<std::string> SolutionParserVisualStudio::findIncludePaths()
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

		std::vector<TiXmlElement*> nodes = SolutionParserUtility::getAllTagsByName(doc.RootElement(), "AdditionalIncludeDirectories");

		for (unsigned int j = 0; j < nodes.size(); j++)
		{
			std::string path = nodes[j]->GetText();

			includePaths.push_back(path);
		}
	}

	includePaths = seperateIncludePaths(includePaths);

	std::set<std::string> s(includePaths.begin(), includePaths.end());
	includePaths.assign(s.begin(), s.end());

	includePaths = SolutionParserUtility::resolveEnvironmentVariables(includePaths);
	includePaths = makePathsAbsolute(includePaths);

	return includePaths;
}

std::vector<std::string> SolutionParserVisualStudio::getProjectBlocks(const std::string& solution) const
{
	std::vector<std::string> blocks;

	std::string subSolution = solution;

	std::string solutionOpenTag = "\nProject";
	std::string solutionCloseTag = "\nEndProject";

	size_t blockStart = subSolution.find(solutionOpenTag, 0);
	size_t blockEnd = subSolution.find(solutionCloseTag, 0);

	while (blockStart != std::string::npos && blockEnd != std::string::npos)
	{
		std::string block = subSolution.substr(blockStart, blockEnd - blockStart + solutionCloseTag.size());

		if (subSolution.size() > blockEnd + solutionCloseTag.size())
		{
			subSolution = subSolution.substr(blockEnd + solutionCloseTag.size());
		}
		else
		{
			subSolution = "";
		}

		blockStart = subSolution.find(solutionOpenTag);
		blockEnd = subSolution.find(solutionCloseTag);
		
		blocks.push_back(block);
	}

	return blocks;
}

std::string SolutionParserVisualStudio::getProjectFilePath(const std::string& projectBlock) const
{
	std::string projectFilePath;

	std::string fileEnding = "vcxproj";

	size_t pos = projectBlock.find(fileEnding);

	if (pos != std::string::npos)
	{
		std::string searchArea = projectBlock.substr(0, pos + fileEnding.size());
		pos = searchArea.find_last_of("\"");
		projectFilePath = searchArea.substr(pos + 1);
	}

	return projectFilePath;
}

std::vector<std::string> SolutionParserVisualStudio::seperateIncludePaths(const std::vector<std::string>& includePaths) const
{
	std::vector<std::string> seperatedPaths;
	
	for (unsigned int i = 0; i < includePaths.size(); i++)
	{
		std::vector<std::string> paths = seperateIncludePaths(includePaths[i]);
		for (unsigned int j = 0; j < paths.size(); j++)
		{
			seperatedPaths.push_back(paths[j]);
		}
	}

	return seperatedPaths;
}

std::vector<std::string> SolutionParserVisualStudio::seperateIncludePaths(const std::string& includePaths) const
{
	std::string paths = includePaths;
	std::string seperator = ";";

	std::vector<std::string> seperatedPaths;

	size_t pos = includePaths.find(seperator);

	while (pos != std::string::npos)
	{
		std::string path = paths.substr(0, pos);
		paths = paths.substr(pos + 1);

		seperatedPaths.push_back(path);

		pos = paths.find(seperator);
	}

	return seperatedPaths;
}
