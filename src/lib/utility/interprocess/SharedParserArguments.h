#ifndef SHARED_PARSER_ARGUMENTS_H
#define SHARED_PARSER_ARGUMENTS_H

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>

#include "utility/file/FilePath.h"

class SharedParserArguments
{
public:
	typedef boost::interprocess::allocator<void, boost::interprocess::managed_shared_memory::segment_manager> VoidAllocator;

	SharedParserArguments(const VoidAllocator& allocator);
	~SharedParserArguments();

	void setLogErrors(const bool logErrors);
	bool getLogErrors() const;

	
	void setLanguage(const std::string& language);
	std::string getLanguage() const;

	
	void setLanguageStandard(const std::string& languageStandard);
	std::string getLanguageStandard() const;

	void setCompilationDatabasePath(const std::string& compilationDatabasePath);
	std::string getCompilationDatabasePath() const;

	void setJavaClassPaths(std::vector<FilePath> javaClassPaths);
	std::vector<FilePath> getJavaClassPaths() const;

	void setHeaderSearchPaths(std::vector<FilePath> headerSearchPaths);
	std::vector<FilePath> getHeaderSearchPaths() const;

	void setSystemHeaderSearchPaths(std::vector<FilePath> systemHeaderSearchPaths);
	std::vector<FilePath> getSystemHeaderSearchPaths() const;

	void setFrameworkSearchPaths(std::vector<FilePath> frameworkSearchPaths);
	std::vector<FilePath> getFrameworkSearchPaths() const;

	void setCompilerFlags(std::vector<std::string> compilerFlags);
	std::vector<std::string> getCompilerFlags() const;

private:
	typedef boost::interprocess::allocator<char, boost::interprocess::managed_shared_memory::segment_manager> CharAllocator;
	typedef boost::interprocess::basic_string<char, std::char_traits<char>, CharAllocator> SharedString;
	typedef boost::interprocess::allocator<SharedString, boost::interprocess::managed_shared_memory::segment_manager> StringAllocator;
	typedef boost::interprocess::vector<SharedString, StringAllocator> SharedStringVector;

	bool m_logErrors;

	SharedString m_language;
	SharedString m_languageStandard;
	SharedString m_compilationDatabasePath;

	SharedStringVector m_javaClassPaths;
	SharedStringVector m_headerSearchPaths;
	SharedStringVector m_systemHeaderSearchPaths;
	SharedStringVector m_frameworkSearchPaths;
	SharedStringVector m_compilerFlags;
};

#endif // SHARED_PARSER_ARGUMENTS_H
