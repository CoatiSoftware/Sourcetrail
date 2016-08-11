#include "SolutionParserVisualStudio.h"

#include <algorithm>
#include <cstdlib>
#include <set>

#include "boost/filesystem/path.hpp"
#include "boost/filesystem.hpp"

#include "SolutionParserUtility.h"

#include "settings/CxxProjectSettings.h"
#include "utility/logging/logging.h"

SolutionParserVisualStudio::SolutionParserVisualStudio()
	: m_compatibilityFlags()
{
	m_compatibilityFlags.push_back("-fms-extensions");
	m_compatibilityFlags.push_back("-fms-compatibility");
	m_compatibilityFlags.push_back("-fms-compatibility-version=19.00");

	SolutionParserUtility::m_ideMacros.clear();

	SolutionParserUtility::m_ideMacros.push_back("RemoteMachine");
	SolutionParserUtility::m_ideMacros.push_back("Configuration");
	SolutionParserUtility::m_ideMacros.push_back("Platform");
	SolutionParserUtility::m_ideMacros.push_back("ParentName");
	SolutionParserUtility::m_ideMacros.push_back("RootNameSpace");
	SolutionParserUtility::m_ideMacros.push_back("IntDir");
	SolutionParserUtility::m_ideMacros.push_back("OutDir");
	SolutionParserUtility::m_ideMacros.push_back("DevEnvDir");
	SolutionParserUtility::m_ideMacros.push_back("InputPath");
	SolutionParserUtility::m_ideMacros.push_back("InputName");
	SolutionParserUtility::m_ideMacros.push_back("InputFileName");
	SolutionParserUtility::m_ideMacros.push_back("InputExt");
	SolutionParserUtility::m_ideMacros.push_back("ProjectDir");
	SolutionParserUtility::m_ideMacros.push_back("ProjectPath");
	SolutionParserUtility::m_ideMacros.push_back("ProjectName");
	SolutionParserUtility::m_ideMacros.push_back("ProjectFileName");
	SolutionParserUtility::m_ideMacros.push_back("ProjectExt");
	SolutionParserUtility::m_ideMacros.push_back("SolutionDir");
	SolutionParserUtility::m_ideMacros.push_back("SolutionPath");
	SolutionParserUtility::m_ideMacros.push_back("SolutionName");
	SolutionParserUtility::m_ideMacros.push_back("SolutionExt");
	SolutionParserUtility::m_ideMacros.push_back("TargetDir");
	SolutionParserUtility::m_ideMacros.push_back("TargetPath");
	SolutionParserUtility::m_ideMacros.push_back("TargetName");
	SolutionParserUtility::m_ideMacros.push_back("TargetFileName");
	SolutionParserUtility::m_ideMacros.push_back("VSInstallDir");
	SolutionParserUtility::m_ideMacros.push_back("VCInstallDir");
	SolutionParserUtility::m_ideMacros.push_back("FrameworkDir");
	SolutionParserUtility::m_ideMacros.push_back("FrameworkVersion");
	SolutionParserUtility::m_ideMacros.push_back("FrameworkSDKDir");
	SolutionParserUtility::m_ideMacros.push_back("WebDeployPath");
	SolutionParserUtility::m_ideMacros.push_back("WebDeployRoot");
	SolutionParserUtility::m_ideMacros.push_back("SafeParentName");
	SolutionParserUtility::m_ideMacros.push_back("SafeInputName");
	SolutionParserUtility::m_ideMacros.push_back("SafeRootNamespace");
	SolutionParserUtility::m_ideMacros.push_back("FxCopDir");
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

std::vector<std::string> SolutionParserVisualStudio::getCompileFlags()
{
	std::vector<std::string> compilerFlags;

	std::vector<std::string> projectFiles = getProjectFiles();

	std::vector<std::string> validExtensions;
	validExtensions.push_back(".c");
	validExtensions.push_back(".cc");
	validExtensions.push_back(".cpp");
	validExtensions.push_back(".h");
	validExtensions.push_back(".hh");
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

		std::vector<TiXmlElement*> nodes = SolutionParserUtility::getAllTagsByName(doc.RootElement(), "PreprocessorDefinitions");

		for (unsigned int j = 0; j < nodes.size(); j++)
		{
			compilerFlags.push_back(std::string(nodes[j]->GetText()));
		}
	}

	compilerFlags = seperateCompilerFlags(compilerFlags);

	for (unsigned int i = 0; i < compilerFlags.size(); i++)
	{
		compilerFlags[i] = "-D " + compilerFlags[i];
	}

	for (unsigned int i = 0; i < m_compatibilityFlags.size(); i++)
	{
		compilerFlags.push_back(m_compatibilityFlags[i]);
	}

	std::set<std::string> s(compilerFlags.begin(), compilerFlags.end());
	compilerFlags.assign(s.begin(), s.end());

	return compilerFlags;
}

std::shared_ptr<ProjectSettings> SolutionParserVisualStudio::getProjectSettings(const std::string& solutionFilePath)
{
	LOG_INFO_STREAM(<< "Starting to parse VS solution");

	LOG_INFO_STREAM(<< "Opening solution file");

	openSolutionFile(solutionFilePath);

	SolutionParserUtility::m_ideMacroValues.clear();
	SolutionParserUtility::m_ideMacroValues["SolutionPath"] = solutionFilePath;
	SolutionParserUtility::m_ideMacroValues["SolutionDir"] = m_solutionPath.substr(0, m_solutionPath.size() - 1); // remove trailing slash, would cause problems during path resolving
	SolutionParserUtility::m_ideMacroValues["SolutionName"] = getSolutionName();
	SolutionParserUtility::m_ideMacroValues["SolutionFileName"] = m_solutionName;

	LOG_INFO_STREAM(<< "Setting meta information");

	std::shared_ptr<CxxProjectSettings> settings = std::make_shared<CxxProjectSettings>(getSolutionName(), getSolutionPath());
	settings->setVisualStudioSolutionPath(solutionFilePath);

	LOG_INFO_STREAM(<< "Parsing project files");

	LOG_INFO_STREAM(<< "Setting source files");

	std::vector<std::string> sourceFiles = getProjectItems();
	std::vector<FilePath> sourcePaths;
	for (const std::string& p : sourceFiles)
	{
		sourcePaths.push_back(FilePath(p));
	}

	LOG_INFO_STREAM(<< "Parsing include files");

	std::vector<std::string> includePaths = getIncludePaths();

	std::vector<FilePath> headerPaths;
	for (const std::string& p : includePaths)
	{
		headerPaths.push_back(FilePath(p));
	}

	LOG_INFO_STREAM(<< "Setting compile flags");

	std::vector<std::string> compilerFlags = getCompileFlags();
	settings->setCompilerFlags(compilerFlags);

	settings->setSourcePaths(sourcePaths);
	settings->setHeaderSearchPaths(headerPaths);

	LOG_INFO_STREAM(<< "Done parsing VS solution");

	return settings;
}

std::string SolutionParserVisualStudio::getIdeName() const
{
	return "Visual Studio";
}

std::string SolutionParserVisualStudio::getButtonText() const
{
	return "from Visual\nStudio Solution";
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
	validFileExtensions.push_back(".cc");
	validFileExtensions.push_back(".cpp");
	validFileExtensions.push_back(".h");
	validFileExtensions.push_back(".hh");
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

		setProjectMacros(projectFilesNames[i]);

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

		std::vector<std::string> filePaths;

		if (root != NULL)
		{
			for (TiXmlElement* child = root->FirstChildElement(); child != NULL; child = child->NextSiblingElement())
			{
				std::string filePath = child->Attribute("Include");

				if (boost::filesystem::exists(filePath) && SolutionParserUtility::checkValidFileExtension(filePath, validFileExtensions))
				{
					filePaths.push_back(filePath);
					continue;
				}

				if (relativeProjectPaths[i].size() > 0)
				{
					filePath = relativeProjectPaths[i] + "/" + filePath;
				}

				if (SolutionParserUtility::checkValidFileExtension(filePath, validFileExtensions))
				{
					filePaths.push_back(filePath);
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
					filePaths.push_back(filePath);
					continue;
				}

				if (relativeProjectPaths[i].size() > 0)
				{
					filePath = relativeProjectPaths[i] + "/" + filePath;
				}

				if (SolutionParserUtility::checkValidFileExtension(filePath, validFileExtensions))
				{
					filePaths.push_back(filePath);
				}
			}
		}

		std::set<std::string> s(filePaths.begin(), filePaths.end());
		filePaths.assign(s.begin(), s.end());

		filePaths = SolutionParserUtility::resolveEnvironmentVariables(filePaths);

		for (unsigned int j = 0; j < filePaths.size(); j++)
		{
			projectItems.push_back(filePaths[j]);
		}
	}

	std::set<std::string> s(projectItems.begin(), projectItems.end());
	projectItems.assign(s.begin(), s.end());

	LOG_INFO_STREAM(<< "Found " << projectItems.size() << " code files");

	// projectItems = SolutionParserUtility::resolveEnvironmentVariables(projectItems);
	projectItems = makePathsAbsolute(projectItems);

	return projectItems;
}

std::vector<std::string> SolutionParserVisualStudio::findIncludePaths()
{
	std::vector<std::string> includePaths;

	std::vector<std::string> projectFiles = getProjectFiles();
	std::vector<std::string> projectFilesNames = getProjects();

	std::vector<std::string> validExtensions;
	validExtensions.push_back(".c");
	validExtensions.push_back(".cc");
	validExtensions.push_back(".cpp");
	validExtensions.push_back(".h");
	validExtensions.push_back(".hh");
	validExtensions.push_back(".hpp");

	for (unsigned int i = 0; i < projectFiles.size(); i++)
	{
		TiXmlDocument doc;
		doc.Parse(projectFiles[i].c_str(), 0, TIXML_ENCODING_UTF8);

		setProjectMacros(projectFilesNames[i]);

		std::string error = doc.ErrorDesc();

		if (error.length() > 0)
		{
			LOG_ERROR_STREAM(<< "Failed to parse project file " << i << ": " << error);
			continue;
		}

		std::vector<TiXmlElement*> nodes = SolutionParserUtility::getAllTagsByName(doc.RootElement(), "AdditionalIncludeDirectories");

		std::vector<std::string> paths;

		for (unsigned int j = 0; j < nodes.size(); j++)
		{
			std::string path = nodes[j]->GetText();

			paths.push_back(path);
		}

		paths = seperateIncludePaths(paths);

		std::set<std::string> s(paths.begin(), paths.end());
		paths.assign(s.begin(), s.end());

		paths = SolutionParserUtility::resolveEnvironmentVariables(paths);

		for (unsigned int j = 0; j < paths.size(); j++)
		{
			includePaths.push_back(paths[j]);
		}
	}

	// includePaths = seperateIncludePaths(includePaths);

	std::set<std::string> s(includePaths.begin(), includePaths.end());
	includePaths.assign(s.begin(), s.end());

	LOG_INFO_STREAM(<< "Found " << includePaths.size() << " additional include paths");

	// includePaths = SolutionParserUtility::resolveEnvironmentVariables(includePaths);
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

	if (paths.size() > 0)
	{
		seperatedPaths.push_back(paths);
	}

	return seperatedPaths;
}

std::vector<std::string> SolutionParserVisualStudio::seperateCompilerFlags(const std::vector<std::string>& compilerFlags) const
{
	std::vector<std::string> seperatedFlags;

	for (unsigned int i = 0; i < compilerFlags.size(); i++)
	{
		std::vector<std::string> paths = seperateCompilerFlags(compilerFlags[i]);
		for (unsigned int j = 0; j < paths.size(); j++)
		{
			seperatedFlags.push_back(paths[j]);
		}
	}

	return seperatedFlags;
}

std::vector<std::string> SolutionParserVisualStudio::seperateCompilerFlags(const std::string& compilerFlags) const
{
	std::string flags = compilerFlags;
	std::string seperator = ";";

	std::vector<std::string> seperatedFlags;

	size_t pos = flags.find(seperator);

	while (pos != std::string::npos)
	{
		std::string flag = flags.substr(0, pos);
		flags = flags.substr(pos + 1);

		seperatedFlags.push_back(flag);

		pos = flags.find(seperator);
	}

	return seperatedFlags;
}

void SolutionParserVisualStudio::setProjectMacros(const std::string& projectName)
{
	std::string solutionPath = getSolutionPath();

	std::string projectNameBase = projectName;

	std::replace(solutionPath.begin(), solutionPath.end(), '/', '\\');
	std::replace(projectNameBase.begin(), projectNameBase.end(), '/', '\\');

	std::string projectPath = solutionPath + projectNameBase;

	std::string projectDirectory = "";
	std::string projectNameOnly = "";

	size_t pos = projectNameBase.find_last_of('\\');
	if (pos != std::string::npos)
	{
		projectDirectory = projectNameBase.substr(0, pos);
		projectNameOnly = projectNameBase.substr(pos + 1);
	}
	else
	{
		projectNameOnly = projectNameBase;
	}

	std::string projectExtension = "";

	pos = projectNameOnly.find_last_of('.');
	if (pos != std::string::npos)
	{
		projectExtension = projectNameOnly.substr(pos + 1);
		projectNameOnly = projectNameOnly.substr(0, pos);
	}

	projectPath = SolutionParserUtility::findAndResolveEnvironmentVariable(projectPath);
	projectPath = SolutionParserUtility::makePathCanonical(projectPath);

	std::string projectDir = "";

	pos = projectPath.find_last_of('\\');
	if (pos != std::string::npos)
	{
		projectDir = projectPath.substr(0, pos+1);
	}

	SolutionParserUtility::m_ideMacroValues["ProjectDir"] = projectDir;
	SolutionParserUtility::m_ideMacroValues["ProjectPath"] = projectPath;
	SolutionParserUtility::m_ideMacroValues["ProjectName"] = projectNameOnly;
	SolutionParserUtility::m_ideMacroValues["ProjectFileName"] = projectNameOnly + "." + projectExtension;
	SolutionParserUtility::m_ideMacroValues["ProjectExt"] = projectExtension;
}
