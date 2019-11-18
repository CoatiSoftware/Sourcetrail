#ifndef CODEBLOCKS_UNIT_H
#define CODEBLOCKS_UNIT_H

#include <memory>
#include <set>

#include "CodeblocksCompilerVarType.h"

class FilePath;
class TiXmlElement;

namespace Codeblocks
{
class Unit
{
public:
	static std::string getXmlElementName();
	static std::shared_ptr<Unit> create(const TiXmlElement* element);

	FilePath getCanonicalFilePath(const FilePath& projectFileDirectory) const;
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
}	 // namespace Codeblocks

#endif	  // CODEBLOCKS_UNIT_H
