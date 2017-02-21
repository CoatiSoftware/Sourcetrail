#include "SharedParserArguments.h"

SharedParserArguments::SharedParserArguments(const VoidAllocator& allocator)
	: m_logErrors(false)
	, m_language("", allocator)
	, m_languageStandard("", allocator)
	, m_compilationDatabasePath("", allocator)
	, m_javaClassPaths(allocator)
	, m_headerSearchPaths(allocator)
	, m_systemHeaderSearchPaths(allocator)
	, m_frameworkSearchPaths(allocator)
	, m_compilerFlags(allocator)
{
}

SharedParserArguments::~SharedParserArguments()
{
}

void SharedParserArguments::setLogErrors(const bool logErrors)
{
	m_logErrors = logErrors;
}

bool SharedParserArguments::getLogErrors() const
{
	return m_logErrors;
}

void SharedParserArguments::setLanguage(const std::string& language)
{
	m_language = language.c_str();
}

std::string SharedParserArguments::getLanguage() const
{
	return std::string(m_language.c_str());
}

void SharedParserArguments::setLanguageStandard(const std::string& languageStandard)
{
	m_languageStandard = languageStandard.c_str();
}

std::string SharedParserArguments::getLanguageStandard() const
{
	return std::string(m_languageStandard.c_str());
}

void SharedParserArguments::setCompilationDatabasePath(const std::string& compilationDatabasePath)
{
	m_compilationDatabasePath = compilationDatabasePath.c_str();
}

std::string SharedParserArguments::getCompilationDatabasePath() const
{
	return std::string(m_compilationDatabasePath.c_str());
}

void SharedParserArguments::setJavaClassPaths(std::vector<FilePath> javaClassPaths)
{
	m_javaClassPaths.clear();
	for (unsigned int i = 0; i < javaClassPaths.size(); i++)
	{
		SharedString path(m_javaClassPaths.get_allocator());
		path = javaClassPaths[i].str().c_str();
		m_javaClassPaths.push_back(path);
	}
}

std::vector<FilePath> SharedParserArguments::getJavaClassPaths() const
{
	std::vector<FilePath> result;

	for (unsigned int i = 0; i < m_javaClassPaths.size(); i++)
	{
		result.push_back(FilePath(m_javaClassPaths[i].c_str()));
	}

	return result;
}

void SharedParserArguments::setHeaderSearchPaths(std::vector<FilePath> headerSearchPaths)
{
	m_headerSearchPaths.clear();
	for (unsigned int i = 0; i < headerSearchPaths.size(); i++)
	{
		SharedString path(m_headerSearchPaths.get_allocator());
		path = headerSearchPaths[i].str().c_str();
		m_headerSearchPaths.push_back(path);
	}
}

std::vector<FilePath> SharedParserArguments::getHeaderSearchPaths() const
{
	std::vector<FilePath> result;

	for (unsigned int i = 0; i < m_headerSearchPaths.size(); i++)
	{
		result.push_back(FilePath(m_headerSearchPaths[i].c_str()));
	}

	return result;
}

void SharedParserArguments::setSystemHeaderSearchPaths(std::vector<FilePath> systemHeaderSearchPaths)
{
	m_systemHeaderSearchPaths.clear();
	for (unsigned int i = 0; i < systemHeaderSearchPaths.size(); i++)
	{
		SharedString path(m_systemHeaderSearchPaths.get_allocator());
		path = systemHeaderSearchPaths[i].str().c_str();
		m_systemHeaderSearchPaths.push_back(path);
	}
}

std::vector<FilePath> SharedParserArguments::getSystemHeaderSearchPaths() const
{
	std::vector<FilePath> result;

	for (unsigned int i = 0; i < m_systemHeaderSearchPaths.size(); i++)
	{
		result.push_back(FilePath(m_systemHeaderSearchPaths[i].c_str()));
	}

	return result;
}

void SharedParserArguments::setFrameworkSearchPaths(std::vector<FilePath> frameworkSearchPaths)
{
	m_frameworkSearchPaths.clear();
	for (unsigned int i = 0; i < frameworkSearchPaths.size(); i++)
	{
		SharedString path(m_frameworkSearchPaths.get_allocator());
		path = frameworkSearchPaths[i].str().c_str();
		m_frameworkSearchPaths.push_back(path);
	}
}

std::vector<FilePath> SharedParserArguments::getFrameworkSearchPaths() const
{
	std::vector<FilePath> result;

	for (unsigned int i = 0; i < m_frameworkSearchPaths.size(); i++)
	{
		result.push_back(FilePath(m_frameworkSearchPaths[i].c_str()));
	}

	return result;
}

void SharedParserArguments::setCompilerFlags(std::vector<std::string> compilerFlags)
{
	m_compilerFlags.clear();
	for (unsigned int i = 0; i < compilerFlags.size(); i++)
	{
		SharedString path(m_compilerFlags.get_allocator());
		path = compilerFlags[i].c_str();
		m_compilerFlags.push_back(path);
	}
}

std::vector<std::string> SharedParserArguments::getCompilerFlags() const
{
	std::vector<std::string> result;

	for (unsigned int i = 0; i < m_compilerFlags.size(); i++)
	{
		result.push_back(m_compilerFlags[i].c_str());
	}

	return result;
}
