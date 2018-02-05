#ifndef CXX_VARIABLE_DECL_NAME_H
#define CXX_VARIABLE_DECL_NAME_H

#include <memory>
#include <vector>

#include "data/parser/cxx/name/CxxDeclName.h"
#include "data/parser/cxx/name/CxxTypeName.h"

class CxxVariableDeclName: public CxxDeclName
{
public:
	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxVariableDeclName(
	//	const std::wstring& name,
	//	const std::vector<std::wstring>& templateParameterNames,
	//	std::shared_ptr<CxxTypeName> typeName,
	//	bool isStatic
	//);

	CxxVariableDeclName(
		std::wstring&& name,
		std::vector<std::wstring>&& templateParameterNames,
		std::shared_ptr<CxxTypeName> typeName,
		bool isStatic
	);

	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxVariableDeclName(
	//	const std::wstring& name,
	//	const std::vector<std::wstring>& templateParameterNames,
	//	std::shared_ptr<CxxTypeName> typeName,
	//	bool isStatic,
	//	std::shared_ptr<CxxName> parent
	//);

	CxxVariableDeclName(
		std::wstring&& name,
		std::vector<std::wstring>&& templateParameterNames,
		std::shared_ptr<CxxTypeName> typeName,
		bool isStatic,
		std::shared_ptr<CxxName> parent
	);

	virtual NameHierarchy toNameHierarchy() const;

private:
	std::shared_ptr<CxxTypeName> m_typeName;
	bool m_isStatic;
};

#endif // CXX_VARIABLE_DECL_NAME_H
