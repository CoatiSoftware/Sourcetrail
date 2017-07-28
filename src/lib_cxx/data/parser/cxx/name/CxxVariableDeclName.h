#ifndef CXX_VARIABLE_DECL_NAME_H
#define CXX_VARIABLE_DECL_NAME_H

#include <memory>
#include <vector>

#include "data/parser/cxx/name/CxxDeclName.h"
#include "data/parser/cxx/name/CxxTypeName.h"

class CxxVariableDeclName: public CxxDeclName
{
public:
	CxxVariableDeclName(
		std::string name,
		std::vector<std::string> templateParameterNames,
		std::shared_ptr<CxxTypeName> typeName,
		bool isStatic
	);

	CxxVariableDeclName(
		std::string name,
		std::vector<std::string> templateParameterNames,
		std::shared_ptr<CxxTypeName> typeName,
		bool isStatic,
		std::shared_ptr<CxxName> parent
	);

	virtual ~CxxVariableDeclName();

	virtual NameHierarchy toNameHierarchy() const;

private:
	std::shared_ptr<CxxTypeName> m_typeName;
	bool m_isStatic;
};

#endif // CXX_VARIABLE_DECL_NAME_H
