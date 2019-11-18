#include "CxxVariableDeclName.h"

CxxVariableDeclName::CxxVariableDeclName(
	std::wstring name,
	std::vector<std::wstring> templateParameterNames,
	std::unique_ptr<CxxTypeName> typeName,
	bool isStatic)
	: CxxDeclName(std::move(name), std::move(templateParameterNames))
	, m_typeName(std::move(typeName))
	, m_isStatic(isStatic)
{
}

NameHierarchy CxxVariableDeclName::toNameHierarchy() const
{
	std::wstring signaturePrefix;
	if (m_isStatic)
	{
		signaturePrefix += L"static ";
	}
	signaturePrefix += m_typeName->toString();

	NameHierarchy ret = CxxDeclName::toNameHierarchy();
	ret.back().setSignature(std::move(signaturePrefix), L"");
	return ret;
}
