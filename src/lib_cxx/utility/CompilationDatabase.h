#ifndef UTILITY_COMPLIATION_DATABASE_H
#define UTILITY_COMPLIATION_DATABASE_H

#include <vector>

#include "utility/file/FilePath.h"

namespace utility
{
	class CompilationDatabase
	{
	public:
		CompilationDatabase(std::string filename);

		std::vector<FilePath> getAllHeaderPaths();
		std::vector<FilePath> getHeaderPaths();
		std::vector<FilePath> getSystemHeaderPaths();
		std::vector<FilePath> getFrameworkHeaderPaths();

	private:
		std::string m_filename;
		std::vector<FilePath> m_headers;
		std::vector<FilePath> m_systemHeaders;
		std::vector<FilePath> m_frameworkHeaders;

		void getHeaders();
	};

}

#endif // UTILITY_COMPLIATION_DATABASE_H
