#include "utility/CompilationDatabase.h"

#include <set>

#include "clang/Tooling/JSONCompilationDatabase.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "utility/file/FilePath.h"
#include "utility/utilityString.h"
#include "utility/utility.h"

utility::CompilationDatabase::CompilationDatabase(const FilePath& filePath)
	: m_filePath(filePath)
{
	init();
}

std::vector<FilePath> utility::CompilationDatabase::getAllHeaderPaths() const
{
	std::vector<FilePath> paths = utility::concat(m_headers, m_systemHeaders);
	paths = utility::unique(paths);
	return paths;
}

std::vector<FilePath> utility::CompilationDatabase::getHeaderPaths() const
{
	return m_headers;
}

std::vector<FilePath> utility::CompilationDatabase::getSystemHeaderPaths() const
{
	return m_systemHeaders;
}

std::vector<FilePath> utility::CompilationDatabase::getFrameworkHeaderPaths() const
{
	return m_frameworkHeaders;
}

void utility::CompilationDatabase::init()
{
	std::string error;
	std::shared_ptr<clang::tooling::JSONCompilationDatabase> cdb = std::shared_ptr<clang::tooling::JSONCompilationDatabase>(
		clang::tooling::JSONCompilationDatabase::loadFromFile(m_filePath.str(), error, clang::tooling::JSONCommandLineSyntax::AutoDetect)
	);

	std::vector<clang::tooling::CompileCommand> commands = cdb->getAllCompileCommands();
	std::set<FilePath> frameworkHeaders;
	std::set<FilePath> systemHeaders;
	std::set<FilePath> headers;

	{
		const std::string frameworkIncludeFlag = "-iframework";
		const std::string systemIncludeFlag = "-isystem";
		const std::string quoteFlag = "-iquote";
		const std::string includeFlag = "-I";
		for (clang::tooling::CompileCommand& command : commands)
		{
			for (size_t i = 0; i < command.CommandLine.size(); i++)
			{
				std::string argument = command.CommandLine[i];
				if (i + 1 < command.CommandLine.size() && !utility::isPrefix("-", command.CommandLine[i + 1]))
				{
					argument += command.CommandLine[++i];
				}

				if (utility::isPrefix(frameworkIncludeFlag, argument))
				{
					frameworkHeaders.insert(FilePath(utility::trim(argument.substr(frameworkIncludeFlag.size())), command.Directory).canonical());
				}
				else if (utility::isPrefix(systemIncludeFlag, argument))
				{
					systemHeaders.insert(FilePath(utility::trim(argument.substr(systemIncludeFlag.size())), command.Directory).canonical());
				}
				else if (utility::isPrefix(quoteFlag, argument))
				{
					headers.insert(FilePath(utility::trim(argument.substr(quoteFlag.size())), command.Directory).canonical());
				}
				else if (utility::isPrefix(includeFlag, argument))
				{
					headers.insert(FilePath(utility::trim(argument.substr(includeFlag.size())), command.Directory).canonical());
				}
			}
		}
	}

	m_headers = utility::toVector(headers);
	m_frameworkHeaders = utility::toVector(frameworkHeaders);
	m_systemHeaders = utility::toVector(systemHeaders);
}
