#include "data/parser/cxx/name/CxxStaticFunctionDeclName.h"

CxxStaticFunctionDeclName::CxxStaticFunctionDeclName(
	const std::string& name,
	const std::vector<std::string>& templateParameterNames,
	std::shared_ptr<CxxTypeName> returnTypeName,
	const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
	const std::string& translationUnitFileName
)
	: CxxFunctionDeclName(name, templateParameterNames, returnTypeName, parameterTypeNames, false, true)
	, m_translationUnitFileName(translationUnitFileName)
{
}

CxxStaticFunctionDeclName::CxxStaticFunctionDeclName(
	const std::string& name,
	const std::vector<std::string>& templateParameterNames,
	std::shared_ptr<CxxTypeName> returnTypeName,
	const std::vector<std::shared_ptr<CxxTypeName>>& parameterTypeNames,
	const std::string& translationUnitFileName,
	std::shared_ptr<CxxName> parent
)
	: CxxFunctionDeclName(name, templateParameterNames, returnTypeName, parameterTypeNames, false, true, parent)
	, m_translationUnitFileName(translationUnitFileName)
{
}

CxxStaticFunctionDeclName::~CxxStaticFunctionDeclName()
{
}

NameHierarchy CxxStaticFunctionDeclName::toNameHierarchy() const
{
	NameHierarchy ret = CxxFunctionDeclName::toNameHierarchy();
	const NameElement::Signature sig = ret.back()->getSignature();

	std::shared_ptr<NameElement> nameElement = std::make_shared<NameElement>(
		ret.back()->getName(),
		NameElement::Signature(sig.getPrefix(), sig.getPostfix() + " (" + m_translationUnitFileName + ")")
	);

	ret.pop();
	ret.push(nameElement);

	return ret;
}
