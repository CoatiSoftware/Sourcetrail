#include "data/parser/cxx/name/CxxFunctionDeclName.h"

CxxFunctionDeclName::CxxFunctionDeclName(
	std::string name,
	std::vector<std::string> templateParameterNames,
	std::shared_ptr<CxxTypeName> returnTypeName,
	std::vector<std::shared_ptr<CxxTypeName>> parameterTypeNames,
	bool isConst,
	bool isStatic
)
	: CxxDeclName(name, templateParameterNames)
	, m_returnTypeName(returnTypeName)
	, m_parameterTypeNames(parameterTypeNames)
	, m_isConst(isConst)
	, m_isStatic(isStatic)
{
}

CxxFunctionDeclName::CxxFunctionDeclName(
	std::string name,
	std::vector<std::string> templateParameterNames,
	std::shared_ptr<CxxTypeName> returnTypeName,
	std::vector<std::shared_ptr<CxxTypeName>> parameterTypeNames,
	bool isConst,
	bool isStatic,
	std::shared_ptr<CxxName> parent
)
	: CxxDeclName(name, templateParameterNames, parent)
	, m_returnTypeName(returnTypeName)
	, m_parameterTypeNames(parameterTypeNames)
	, m_isConst(isConst)
	, m_isStatic(isStatic)
{
}

CxxFunctionDeclName::~CxxFunctionDeclName()
{
}

NameHierarchy CxxFunctionDeclName::toNameHierarchy() const
{
	std::string signaturePrefix = "";
	if (m_isStatic)
	{
		signaturePrefix += "static ";
	}
	signaturePrefix += CxxTypeName::makeUnsolvedIfNull(m_returnTypeName)->toString();

	std::string signaturePostfix = "(";
	for (size_t i = 0; i < m_parameterTypeNames.size(); i++)
	{
		if (i != 0)
		{
			signaturePostfix += ", ";
		}
		signaturePostfix += CxxTypeName::makeUnsolvedIfNull(m_parameterTypeNames[i])->toString();
	}
	signaturePostfix += ")";
	if (m_isConst)
	{
		signaturePostfix += " const";
	}

	NameHierarchy ret = CxxDeclName::toNameHierarchy();
	std::shared_ptr<NameElement> nameElement = std::make_shared<NameElement>(
		ret.back()->getName(),
		NameElement::Signature(signaturePrefix, signaturePostfix)
	);

	ret.pop();
	ret.push(nameElement);

	return ret;
}
