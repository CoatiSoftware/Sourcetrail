#ifndef CXX_DECL_NAME_H
#define CXX_DECL_NAME_H

#include <memory>
#include <string>
#include <vector>

#include "CxxName.h"
#include "NameHierarchy.h"

class CxxDeclName: public CxxName
{
public:
	CxxDeclName(std::wstring name);

	CxxDeclName(std::wstring name, std::vector<std::wstring> templateParameterNames);

	NameHierarchy toNameHierarchy() const override;

	const std::wstring& getName() const;
	const std::vector<std::wstring>& getTemplateParameterNames() const;

private:
	const std::wstring m_name;
	const std::vector<std::wstring> m_templateParameterNames;
};

#endif	  // CXX_DECL_NAME_H
