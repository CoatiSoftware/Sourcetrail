#include "utility/codeblocks/CodeblocksCompilerVarType.h"

namespace Codeblocks
{
	std::string compilerVarTypeToString(CompilerVarType v)
	{
		switch (v)
		{
		case COMPILER_VAR_CPP:
			return "CPP";
		case COMPILER_VAR_C:
			return "C";
		case COMPILER_VAR_WINDRES:
			return "WINDRES";
		default:
			break;
		}
		return "unknown";
	}

	CompilerVarType stringToCompilerVarType(const std::string& v)
	{
		if (v == compilerVarTypeToString(COMPILER_VAR_CPP))
		{
			return COMPILER_VAR_CPP;
		}
		else if (v == compilerVarTypeToString(COMPILER_VAR_C))
		{
			return COMPILER_VAR_C;
		}
		else if (v == compilerVarTypeToString(COMPILER_VAR_WINDRES))
		{
			return COMPILER_VAR_WINDRES;
		}
		return COMPILER_VAR_UNKNOWN;
	}
}
