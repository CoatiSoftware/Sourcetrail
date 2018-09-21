#ifndef CXX_FUNCTION_DECL_NAME_H
#define CXX_FUNCTION_DECL_NAME_H

#include <memory>
#include <vector>

#include "CxxDeclName.h"
#include "CxxTypeName.h"

class CxxFunctionDeclName
	: public CxxDeclName
{
public:
	CxxFunctionDeclName(
		std::wstring name,
		std::vector<std::wstring> templateParameterNames,
		std::unique_ptr<CxxTypeName> returnTypeName,
		std::vector<std::unique_ptr<CxxTypeName>> parameterTypeNames,
		const bool isConst,
		const bool isStatic
	);

	NameHierarchy toNameHierarchy() const override;

private:
	const std::unique_ptr<CxxTypeName> m_returnTypeName;
	const std::vector<std::unique_ptr<CxxTypeName>> m_parameterTypeNames;
	const bool m_isConst;
	const bool m_isStatic;
};

#endif // CXX_FUNCTION_DECL_NAME_H
