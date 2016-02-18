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

private:
    std::shared_ptr<clang::tooling::JSONCompilationDatabase> m_database;
    clang::tooling::JSONCompilationDatabase* getDatabase();
    std::string getIncludePath(const std::string& argument, const std::string& dir);
};


#endif //COATI_SOLUTIONPARSERCOMPILATIONDATABASE_H
