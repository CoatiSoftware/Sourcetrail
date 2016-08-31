#include "utility/CompilationDatabase.h"

#include <set>
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
}
