#ifndef CXX_COMPILATION_DATABASE_SINGLE_H
#define CXX_COMPILATION_DATABASE_SINGLE_H

#include <clang/Tooling/CompilationDatabase.h>

class CxxCompilationDatabaseSingle: public clang::tooling::CompilationDatabase
{
public:
	CxxCompilationDatabaseSingle(const clang::tooling::CompileCommand& command);

	std::vector<clang::tooling::CompileCommand> getCompileCommands(llvm::StringRef FilePath) const override;
	std::vector<std::string> getAllFiles() const override;
	std::vector<clang::tooling::CompileCommand> getAllCompileCommands() const override;

private:
	clang::tooling::CompileCommand m_command;
};

#endif	  // CXX_COMPILATION_DATABASE_SINGLE_H
