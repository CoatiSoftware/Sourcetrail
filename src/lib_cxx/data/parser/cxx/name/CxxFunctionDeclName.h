#ifndef CXX_FUNCTION_DECL_NAME_H
#define CXX_FUNCTION_DECL_NAME_H

#include <memory>
#include <vector>

#include "data/parser/cxx/name/CxxDeclName.h"
#include "data/parser/cxx/name/CxxTypeName.h"

class CxxFunctionDeclName: public CxxDeclName
{
public:
	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxFunctionDeclName(
	//	const std::string& name,
	//	const std::vector<std::string>& templateParameterNames,
	//	std::shared_ptr<CxxTypeName> returnTypeName,
	//	const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
	//	const bool isConst,
	//	const bool isStatic
	//);

	CxxFunctionDeclName(
		std::string&& name,
		std::vector<std::string>&& templateParameterNames,
		std::shared_ptr<CxxTypeName> returnTypeName,
		std::vector<std::shared_ptr<CxxTypeName>>&& parameterTypeNames,
		const bool isConst,
		const bool isStatic
	);

	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxFunctionDeclName(
	//	const std::string& name,
	//	const std::vector<std::string>& templateParameterNames,
	//	std::shared_ptr<CxxTypeName> returnTypeName,
	//	const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
	//	const bool isConst,
	//	const bool isStatic,
	//	std::shared_ptr<CxxName> parent
	//);

	CxxFunctionDeclName(
		std::string&& name,
		std::vector<std::string>&& templateParameterNames,
		std::shared_ptr<CxxTypeName> returnTypeName,
		std::vector<std::shared_ptr<CxxTypeName>>&& parameterTypeNames,
		const bool isConst,
		const bool isStatic,
		std::shared_ptr<CxxName> parent
	);

	virtual NameHierarchy toNameHierarchy() const;

private:
	std::shared_ptr<CxxTypeName> m_returnTypeName;
	std::vector<std::shared_ptr<CxxTypeName>> m_parameterTypeNames;
	bool m_isConst;
	bool m_isStatic;
};

#endif // CXX_FUNCTION_DECL_NAME_H
