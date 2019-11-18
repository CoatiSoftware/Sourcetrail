#ifndef UTILITY_COMPLIATION_DATABASE_H
#define UTILITY_COMPLIATION_DATABASE_H

#include <string>
#include <vector>

#include "FilePath.h"

namespace utility
{
class CompilationDatabase
{
public:
	CompilationDatabase(const FilePath& filePath);

	std::vector<FilePath> getAllHeaderPaths() const;
	std::vector<FilePath> getHeaderPaths() const;
	std::vector<FilePath> getSystemHeaderPaths() const;
	std::vector<FilePath> getFrameworkHeaderPaths() const;

private:
	void init();

	FilePath m_filePath;
	std::vector<FilePath> m_headers;
	std::vector<FilePath> m_systemHeaders;
	std::vector<FilePath> m_frameworkHeaders;
};

}	 // namespace utility

#endif	  // UTILITY_COMPLIATION_DATABASE_H
