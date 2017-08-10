#include "utility/CompilationDatabase.h"

#include <set>

#include "clang/Tooling/JSONCompilationDatabase.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "utility/file/FilePath.h"
#include "utility/utility.h"

utility::CompilationDatabase::CompilationDatabase(std::string filename)
	: m_filename(filename)
{
	getHeaders();
}

std::vector<FilePath> utility::CompilationDatabase::getAllHeaderPaths()
{
	std::vector<FilePath> paths = utility::concat(m_headers, m_systemHeaders);
	paths = utility::concat(paths, m_frameworkHeaders);
	paths = utility::unique(paths);
	return paths;
}

std::vector<FilePath> utility::CompilationDatabase::getHeaderPaths()
{
	return m_headers;
}

std::vector<FilePath> utility::CompilationDatabase::getSystemHeaderPaths()
{
	return m_systemHeaders;
}

std::vector<FilePath> utility::CompilationDatabase::getFrameworkHeaderPaths()
{
	return m_frameworkHeaders;
}

void  utility::CompilationDatabase::getHeaders()
{
	std::string error;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>
		(clang::tooling::JSONCompilationDatabase::loadFromFile(m_filename, error, clang::tooling::JSONCommandLineSyntax::AutoDetect));

	std::vector<clang::tooling::CompileCommand> commands = cdb->getAllCompileCommands();
	std::set<FilePath> frameworkHeaders;
	std::set<FilePath> systemHeaders;
	std::set<FilePath> headers;

	for (clang::tooling::CompileCommand& command : commands)
	{
		for( size_t i = 0; i < command.CommandLine.size(); i++)
		{
			if( command.CommandLine[i] == "-iframework" )
			{
				frameworkHeaders.insert(FilePath(command.CommandLine[++i], command.Directory));
			}
			if( command.CommandLine[i] == "-isystem" )
			{
				systemHeaders.insert(FilePath(command.CommandLine[++i], command.Directory));
			}
			if( command.CommandLine[i].substr(0,2) == "-I" )
			{
				headers.insert(FilePath(command.CommandLine[i].substr(2), command.Directory));
			}
		}
	}

	m_headers = utility::toVector(headers);
	m_frameworkHeaders = utility::toVector(frameworkHeaders);
	m_systemHeaders = utility::toVector(systemHeaders);
}
