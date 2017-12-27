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
	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxDeclName(
	//	const std::string& name,
	//	const std::vector<std::string>& templateParameterNames
	//);

	CxxDeclName(
		std::string&& name,
		std::vector<std::string>&& templateParameterNames
	);

	// uncomment this constructor if required, but try to use the one using move constructors for the members
	//CxxDeclName(
	//	const std::string& name,
	//	const std::vector<std::string>& templateParameterNames,
	//	std::shared_ptr<CxxName> parent
	//);

	CxxDeclName(
		std::string&& name,
		std::vector<std::string>&& templateParameterNames,
		std::shared_ptr<CxxName> parent
	);

	virtual NameHierarchy toNameHierarchy() const;

	std::string getName() const;
	std::vector<std::string> getTemplateParameterNames() const;

private:
	std::string m_name;
	std::vector<std::string> m_templateParameterNames;
};

#endif // CXX_DECL_NAME_H
