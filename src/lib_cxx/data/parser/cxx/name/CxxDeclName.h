#ifndef CXX_DECL_NAME_H
#define CXX_DECL_NAME_H

#include <memory>
#include <string>
#include <vector>

#include "data/name/NameHierarchy.h"
#include "data/parser/cxx/name/CxxName.h"

class CxxDeclName: public CxxName
{
public:
	CxxDeclName(
		std::string name,
		std::vector<std::string> templateParameterNames
	);

	CxxDeclName(
		std::string name,
		std::vector<std::string> templateParameterNames,
		std::shared_ptr<CxxName> parent
	);

	virtual ~CxxDeclName();
	virtual NameHierarchy toNameHierarchy() const;

	std::string getName() const;
	std::vector<std::string> getTemplateParameterNames() const;

private:
	std::string m_name;
	std::vector<std::string> m_templateParameterNames;
};

#endif // CXX_DECL_NAME_H
