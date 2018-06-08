#ifndef CODEBLOCKS_COMPILER_VAR_TYPE_H
#define CODEBLOCKS_COMPILER_VAR_TYPE_H

#include <string>

namespace Codeblocks
{
	enum CompilerVarType
	{
		COMPILER_VAR_CPP,
		COMPILER_VAR_C,
		COMPILER_VAR_WINDRES,
		COMPILER_VAR_UNKNOWN
	};

	std::string compilerVarTypeToString(CompilerVarType v);
	CompilerVarType stringToCompilerVarType(const std::string& v);
}

#endif // CODEBLOCKS_COMPILER_VAR_TYPE_H
