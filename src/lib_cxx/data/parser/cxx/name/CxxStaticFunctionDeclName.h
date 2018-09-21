#ifndef CXX_STATIC_FUNCTION_DECL_NAME_H
#define CXX_STATIC_FUNCTION_DECL_NAME_H

#include "CxxFunctionDeclName.h"

class CxxStaticFunctionDeclName
	: public CxxFunctionDeclName
{
public:
	CxxStaticFunctionDeclName(
		std::wstring name,
		std::vector<std::wstring> templateParameterNames,
		std::unique_ptr<CxxTypeName> returnTypeName,
		std::vector<std::unique_ptr<CxxTypeName>> parameterTypeNames,
		std::wstring translationUnitFileName
	);

	NameHierarchy toNameHierarchy() const override;

private:
	std::wstring m_translationUnitFileName;
};

#endif // CXX_FUNCTION_DECL_NAME_H
