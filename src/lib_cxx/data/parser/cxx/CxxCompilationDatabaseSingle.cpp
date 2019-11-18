#include "CxxCompilationDatabaseSingle.h"

CxxCompilationDatabaseSingle::CxxCompilationDatabaseSingle(const clang::tooling::CompileCommand& command)
	: m_command(command)
{
}

std::vector<clang::tooling::CompileCommand> CxxCompilationDatabaseSingle::getCompileCommands(
	llvm::StringRef FilePath) const
{
	return getAllCompileCommands();
}

std::vector<std::string> CxxCompilationDatabaseSingle::getAllFiles() const
{
	return std::vector<std::string>(1, m_command.Filename);
}

std::vector<clang::tooling::CompileCommand> CxxCompilationDatabaseSingle::getAllCompileCommands() const
{
	return std::vector<clang::tooling::CompileCommand>(1, m_command);
}
