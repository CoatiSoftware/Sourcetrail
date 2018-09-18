#ifndef CXX_STATIC_FUNCTION_DECL_NAME_H
#define CXX_STATIC_FUNCTION_DECL_NAME_H

#include "CxxFunctionDeclName.h"

class CxxStaticFunctionDeclName: public CxxFunctionDeclName
{
public:
	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxStaticFunctionDeclName(
	//	const std::wstring& name,
	//	const std::vector<std::wstring>& templateParameterNames,
	//	std::shared_ptr<CxxTypeName> returnTypeName,
	//	const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
	//	const std::wstring& translationUnitFileName
	//);

	CxxStaticFunctionDeclName(
		std::wstring&& name,
		std::vector<std::wstring>&& templateParameterNames,
		std::shared_ptr<CxxTypeName> returnTypeName,
		std::vector<std::shared_ptr<CxxTypeName>>&& parameterTypeNames,
		std::wstring&& translationUnitFileName
	);

	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxStaticFunctionDeclName(
	//	const std::wstring& name,
	//	const std::vector<std::wstring>& templateParameterNames,
	//	std::shared_ptr<CxxTypeName> returnTypeName,
	//	const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
	//	const std::wstring& translationUnitFileName,
	//	std::shared_ptr<CxxName> parent
	//);

	CxxStaticFunctionDeclName(
		std::wstring&& name,
		std::vector<std::wstring>&& templateParameterNames,
		std::shared_ptr<CxxTypeName> returnTypeName,
		std::vector<std::shared_ptr<CxxTypeName>>&& parameterTypeNames,
		std::wstring&& translationUnitFileName,
		std::shared_ptr<CxxName> parent
	);

	virtual NameHierarchy toNameHierarchy() const;

private:
	std::wstring m_translationUnitFileName;
};

#endif // CXX_FUNCTION_DECL_NAME_H
