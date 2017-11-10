#ifndef CXX_STATIC_FUNCTION_DECL_NAME_H
#define CXX_STATIC_FUNCTION_DECL_NAME_H

#include "data/parser/cxx/name/CxxFunctionDeclName.h"

class CxxStaticFunctionDeclName: public CxxFunctionDeclName
{
public:
	CxxStaticFunctionDeclName(
		const std::string& name,
		const std::vector<std::string>& templateParameterNames,
		std::shared_ptr<CxxTypeName> returnTypeName,
		const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
		const std::string& translationUnitFileName
	);

	CxxStaticFunctionDeclName(
		const std::string& name,
		const std::vector<std::string>& templateParameterNames,
		std::shared_ptr<CxxTypeName> returnTypeName,
		const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
		const std::string& translationUnitFileName,
		std::shared_ptr<CxxName> parent
	);

	virtual ~CxxStaticFunctionDeclName();

	virtual NameHierarchy toNameHierarchy() const;

private:
	std::string m_translationUnitFileName;
};

#endif // CXX_FUNCTION_DECL_NAME_H
