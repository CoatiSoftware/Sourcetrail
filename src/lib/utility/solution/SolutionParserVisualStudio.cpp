#include "SolutionParserVisualStudio.h"

#include <algorithm>
#include <cstdlib>
#include <set>

#include "boost/filesystem/path.hpp"
#include "boost/filesystem.hpp"

#include "utility/logging/logging.h"

SolutionParserVisualStudio::SolutionParserVisualStudio()
{

}

SolutionParserVisualStudio::~SolutionParserVisualStudio()
{

}

std::string SolutionParserVisualStudio::getSolutionName()
{
	std::string result = "";

	if (m_solutionName.size() > 0)
	{
		size_t pos = m_solutionName.find(".sln");

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

	projectItems = makePathsCanonical(projectItems);

	return projectItems;
}

std::vector<std::string> SolutionParserVisualStudio::getIncludePaths()
{
	std::vector<std::string> includePaths = findIncludePaths();

	includePaths = makePathsCanonical(includePaths);

	return includePaths;
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

		TiXmlElement* root = getFirstTagByNameWithAttribute(doc.RootElement(), "ClCompile", "Include");
		TiXmlElement* headerRoot = getFirstTagByNameWithAttribute(doc.RootElement(), "ClInclude", "Include");
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

				if (boost::filesystem::exists(filePath) && checkValidFileExtension(filePath, validFileExtensions))
				{
					projectItems.push_back(filePath);
					continue;
				}

				if (relativeProjectPaths[i].size() > 0)
				{
					filePath = relativeProjectPaths[i] + "/" + filePath;
				}

				if (checkValidFileExtension(filePath, validFileExtensions))
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

				if (boost::filesystem::exists(filePath) && checkValidFileExtension(filePath, validFileExtensions))
				{
					projectItems.push_back(filePath);
					continue;
				}

				if (relativeProjectPaths[i].size() > 0)
				{
					filePath = relativeProjectPaths[i] + "/" + filePath;
				}

				if (checkValidFileExtension(filePath, validFileExtensions))
				{
					projectItems.push_back(filePath);
				}
			}
		}
	}

	std::set<std::string> s(projectItems.begin(), projectItems.end());
	projectItems.assign(s.begin(), s.end());

	projectItems = resolveEnvironmentVariables(projectItems);
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

		std::vector<TiXmlElement*> nodes = getAllTagsByName(doc.RootElement(), "AdditionalIncludeDirectories");

		for (unsigned int j = 0; j < nodes.size(); j++)
		{
			std::string path = nodes[j]->GetText();

			includePaths.push_back(path);
		}
	}

	includePaths = seperateIncludePaths(includePaths);

	std::set<std::string> s(includePaths.begin(), includePaths.end());
	includePaths.assign(s.begin(), s.end());

	includePaths = resolveEnvironmentVariables(includePaths);
	includePaths = makePathsAbsolute(includePaths);

	return includePaths;
}

bool SolutionParserVisualStudio::checkValidFileExtension(const std::string& file, const std::vector<std::string>& validExtensions)
{
	for (unsigned int i = 0; i < validExtensions.size(); i++)
	{
		size_t pos = file.find(validExtensions[i]);

		if (pos != std::string::npos)
		{
			return true;
		}
	}

	return false;
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

TiXmlElement* SolutionParserVisualStudio::getFirstTagByName(TiXmlElement* root, const std::string& tag)
{
	TiXmlElement* element = root;

	while (element)
	{
		if (element == NULL)
		{
		}

		std::string value = element->Value();

		if (value == tag) // "ClInclude") // || value == "ClCompile")
		{
			if (element->Parent() != NULL)
			{
				return element; // ->Parent()->ToElement();
			}
		}
		
		if (element->FirstChildElement() != NULL)
		{
			element = element->FirstChildElement();
		}
		else if (element->NextSiblingElement() != NULL)
		{
			element = element->NextSiblingElement();
		}
		else
		{
			if (element == NULL)
			{
			}

			while (element->Parent()->ToElement() != NULL && element->Parent()->NextSiblingElement() == NULL)
			{
				TiXmlElement* newElement = element->Parent()->ToElement();

				if (newElement == NULL)
				{
				}

				element = newElement;
			}
			if (element->Parent() != NULL && element->Parent()->NextSiblingElement() != NULL)
			{
				element = element->Parent()->NextSiblingElement();
			}
			else
			{
				return NULL;
			}
		}
	}

	return NULL;
}

std::vector<TiXmlElement*> SolutionParserVisualStudio::getAllTagsByName(TiXmlElement* root, const std::string& tag)
{
	std::vector<TiXmlElement*> nodes;

	TiXmlElement* element = root;

	while (element)
	{
		std::string value = element->Value();

		if (value == tag)
		{
			nodes.push_back(element);
		}

		if (element->FirstChildElement() != NULL)
		{
			element = element->FirstChildElement();
		}
		else if (element->NextSiblingElement() != NULL)
		{
			element = element->NextSiblingElement();
		}
		else
		{
			if (element == NULL)
			{
			}

			while (element->Parent()->ToElement() != NULL && element->Parent()->NextSiblingElement() == NULL)
			{
				TiXmlElement* newElement = element->Parent()->ToElement();

				if (newElement == NULL)
				{
				}

				element = newElement;
			}
			if (element->Parent() != NULL && element->Parent()->NextSiblingElement() != NULL)
			{
				element = element->Parent()->NextSiblingElement();
			}
			else
			{
				break;
			}
		}
	}

	return nodes;
}

TiXmlElement* SolutionParserVisualStudio::getFirstTagByNameWithAttribute(TiXmlElement* root, const std::string& tag, const std::string& attribute)
{
	TiXmlElement* element = root;

	while (element)
	{
		if (element == NULL)
		{
		}

		std::string value = element->Value();

		bool hasAttribute = false;
		if (element->Attribute(attribute.c_str()) != NULL)
		{
			hasAttribute = true;
		}

		if (value == tag && hasAttribute) // "ClInclude") // || value == "ClCompile")
		{
			if (element->Parent() != NULL)
			{
				return element; // ->Parent()->ToElement();
			}
		}

		if (element->FirstChildElement() != NULL)
		{
			element = element->FirstChildElement();
		}
		else if (element->NextSiblingElement() != NULL)
		{
			element = element->NextSiblingElement();
		}
		else
		{
			while (element->Parent()->ToElement() != NULL && element->Parent()->NextSiblingElement() == NULL)
			{
				TiXmlElement* newElement = element->Parent()->ToElement();

				element = newElement;
			}
			if (element->Parent() != NULL && element->Parent()->NextSiblingElement() != NULL)
			{
				element = element->Parent()->NextSiblingElement();
			}
			else
			{
				return NULL;
			}
		}
	}

	return NULL;
}

std::vector<TiXmlElement*> SolutionParserVisualStudio::getAllTagsByNameWithAttribute(TiXmlElement* root, const std::string& tag, const std::string& attribute)
{
	std::vector<TiXmlElement*> nodes;

	TiXmlElement* element = root;

	while (element)
	{
		std::string value = element->Value();

		bool hasAttribute = false;
		if (element->Attribute(attribute.c_str()) != NULL)
		{
			hasAttribute = true;
		}

		if (value == tag && hasAttribute)
		{
			nodes.push_back(element);
		}

		if (element->FirstChildElement() != NULL)
		{
			element = element->FirstChildElement();
		}
		else if (element->NextSiblingElement() != NULL)
		{
			element = element->NextSiblingElement();
		}
		else
		{
			while (element->Parent()->ToElement() != NULL && element->Parent()->NextSiblingElement() == NULL)
			{
				TiXmlElement* newElement = element->Parent()->ToElement();

				element = newElement;
			}
			if (element->Parent() != NULL && element->Parent()->NextSiblingElement() != NULL)
			{
				element = element->Parent()->NextSiblingElement();
			}
			else
			{
				break;
			}
		}
	}

	return nodes;
}

std::vector<std::string> SolutionParserVisualStudio::resolveEnvironmentVariables(const std::vector<std::string>& paths)
{
	std::vector<std::string> resolvedPaths;

	for (unsigned int i = 0; i < paths.size(); i++)
	{
		resolvedPaths.push_back(findAndResolveEnvironmentVariable(paths[i]));
	}

	return resolvedPaths;
}

std::string SolutionParserVisualStudio::findAndResolveEnvironmentVariable(const std::string& path)
{
	std::string resolvedPath;

	size_t pos = path.find("$(");

	if (pos != std::string::npos)
	{
		size_t endPos = path.substr(pos).find(")");

		std::string envVariable = path.substr(pos+2, (endPos-2) - pos);
		std::string envPath = getenv(envVariable.c_str());

		std::string prePath = path.substr(0, pos);
		std::string postPath = path.substr(endPos + 1);

		resolvedPath = prePath + envPath + postPath;
	}
	else
	{
		resolvedPath = path;
	}

	return resolvedPath;
}

std::vector<std::string> SolutionParserVisualStudio::makePathsAbsolute(const std::vector<std::string>& paths)
{
	std::vector<std::string> absolutePaths;

	for (unsigned int i = 0; i < paths.size(); i++)
	{
		std::string path = paths[i];

		boost::filesystem::path boostPath(path);
		if (boostPath.is_relative())
		{
			path = m_solutionPath + path;
		}

		absolutePaths.push_back(path);
	}

	return absolutePaths;
}

std::vector<std::string> SolutionParserVisualStudio::makePathsCanonical(const std::vector<std::string>& paths)
{
	std::vector<std::string> canonicalPaths;

	for (unsigned int i = 0; i < paths.size(); i++)
	{
		try
		{
			boost::filesystem::path canonicalPath = boost::filesystem::canonical(boost::filesystem::path(paths[i]));
			canonicalPaths.push_back(canonicalPath.string());
		}
		catch (std::exception& e)
		{
			std::string what = e.what();
			LOG_WARNING_STREAM(<< e.what());
		}
	}
	
	return canonicalPaths;
}
