/*
#include "utility/solution/SolutionParserCompilationDatabase.h"

#include <set>

#include "clang/Tooling/JSONCompilationDatabase.h"
#include "settings/ProjectSettings.h"
#include "utility/file/FilePath.h"
#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/utility.h"

SolutionParserCompilationDatabase::SolutionParserCompilationDatabase()
{
}

SolutionParserCompilationDatabase::~SolutionParserCompilationDatabase()
{
}

std::string SolutionParserCompilationDatabase::getSolutionName()
{
    return "";
}

std::vector<std::string> SolutionParserCompilationDatabase::getProjects()
{
    return std::vector<std::string>();
}

std::vector<std::string> SolutionParserCompilationDatabase::getProjectItems()
{
    std::vector<std::string> files = getDatabase()->getAllFiles();

    std::vector<std::string> extensions = ProjectSettings::getInstance()->getHeaderExtensions();
    std::vector<FileInfo> fileInfos = FileSystem::getFileInfosFromPaths(m_headerPaths, extensions);

    for (const FileInfo& info : fileInfos)
    {
        files.push_back(info.path.str());
    }

    return files;
}

std::vector<std::string> SolutionParserCompilationDatabase::getIncludePaths()
{
	return m_searchPaths;
}

std::vector<std::string> SolutionParserCompilationDatabase::getFrameworkPaths()
{
	return m_frameworkPaths;
}

void SolutionParserCompilationDatabase::parseDatabase()
{
	std::vector<clang::tooling::CompileCommand> commands = getDatabase()->getAllCompileCommands();

    std::set<std::string> headerPaths;
    std::set<std::string> searchPaths;
	std::set<std::string> frameworkPaths;
    bool insertNext = false;
	bool insertFramework = false;
    for(clang::tooling::CompileCommand command : commands)
    {
        std::string dir = command.Directory;
        for(std::string argument : command.CommandLine)
        {
            if(insertNext)
            {
                searchPaths.insert(getIncludePath(argument, command.Directory));
                insertNext = false;
            }
			if(insertFramework)
			{
				frameworkPaths.insert(getIncludePath(argument,command.Directory));
				insertFramework = false;
			}
            if(argument.substr(0,2) == "-I")
            {
                std::string includePath = getIncludePath(argument.substr(2), command.Directory);
                headerPaths.insert(includePath);
                searchPaths.insert(includePath);
            }
            if(argument == "-isystem")
            {
                insertNext = true;
            }
			if (argument == "-iframework")
			{
				insertFramework = true;
			}
        }
    }

    for(std::string p : headerPaths)
    {
        m_headerPaths.push_back(FilePath(p));
    }

    for(std::string p : searchPaths)
    {
		m_searchPaths.push_back(p);
    }

    for(std::string p : frameworkPaths)
    {
        m_frameworkPaths.push_back(p);
    }
}


std::vector<std::string> SolutionParserCompilationDatabase::getProjectFiles()
{
    return std::vector<std::string>();
}

std::string SolutionParserCompilationDatabase::getIncludePath(const std::string& path, const std::string& dir)
{
    if (FilePath(path).isAbsolute())
    {
        return path;
    }
    return dir + "/" + path;
}

clang::tooling::JSONCompilationDatabase* SolutionParserCompilationDatabase::getDatabase()
{
    if(m_database == nullptr)
    {
        if(m_solutionPath.empty())
        {
            LOG_ERROR("No compilation database file");
        }
        else
        {
            std::string error;
            m_database = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
                    ( clang::tooling::JSONCompilationDatabase::loadFromFile(m_solutionPath + m_solutionName,error) );
        }
        if(m_database == nullptr)
        {
            LOG_ERROR("Could not load compilation database from file: " + m_solutionPath );
        }
    }
    return m_database.get();
}
*/
