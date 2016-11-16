#ifndef CXX_DECL_NAME_H
#define CXX_DECL_NAME_H

#include <memory>
#include <string>
#include <vector>

struct CxxDeclName
{
	std::string m_name;
	std::shared_ptr<CxxDeclName> m_parent;
	std::vector<std::string> m_templateParameterNames;



	private JavaDeclName m_parent = null;
	private String m_name = "";
	private List<String> m_typeParameterNames = null;
	private JavaTypeName m_returnTypeName = null;
	private List<JavaTypeName> m_parameterNames = null;
};

struct CxxFunctionDeclName: public CxxDeclName
{
};

#endif // CXX_DECL_NAME_H
