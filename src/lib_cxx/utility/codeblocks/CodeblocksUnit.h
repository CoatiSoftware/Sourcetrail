#ifndef CODEBLOCKS_UNIT_H
#define CODEBLOCKS_UNIT_H

#include <memory>
#include <set>

#include "utility/codeblocks/CodeblocksCompilerVarType.h"

class TiXmlElement;

namespace Codeblocks
{
	class Unit
	{
	public:
		static std::string getXmlElementName();
		static std::shared_ptr<Unit> create(const TiXmlElement* element);

		std::wstring getFilename() const;
		CompilerVarType getCompilerVar() const;
		bool getCompile() const;
		std::set<std::wstring> getTargetNames() const;

	private:
		Unit();

		std::wstring m_filename;
		CompilerVarType m_compilerVar;
		bool m_compile;
		std::set<std::wstring> m_targetNames;
	};
}

#endif // CODEBLOCKS_UNIT_H
