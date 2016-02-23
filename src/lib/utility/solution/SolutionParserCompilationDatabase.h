#ifndef COATI_SOLUTIONPARSERCOMPILATIONDATABASE_H
#define COATI_SOLUTIONPARSERCOMPILATIONDATABASE_H

#include "utility/solution/ISolutionParser.h"

#include <memory>
#include <string>

class FilePath;
namespace clang
{
    namespace tooling
    {
        class JSONCompilationDatabase;
    }
}

class SolutionParserCompilationDatabase : public ISolutionParser
{
public:
    SolutionParserCompilationDatabase();
    ~SolutionParserCompilationDatabase();

    virtual std::string getSolutionName();
    virtual std::vector<std::string> getProjects();
    virtual std::vector<std::string> getProjectFiles();
    virtual std::vector<std::string> getProjectItems();
    virtual std::vector<std::string> getIncludePaths();
	std::vector<std::string> getFrameworkPaths();

private:
    std::shared_ptr<clang::tooling::JSONCompilationDatabase> m_database;
	void parseDatabase();
	bool m_parsed;
    clang::tooling::JSONCompilationDatabase* getDatabase();
	std::vector<std::string> m_searchPaths;
	std::vector<std::string> m_frameworkPaths;
    std::string getIncludePath(const std::string& argument, const std::string& dir);
};


#endif //COATI_SOLUTIONPARSERCOMPILATIONDATABASE_H
