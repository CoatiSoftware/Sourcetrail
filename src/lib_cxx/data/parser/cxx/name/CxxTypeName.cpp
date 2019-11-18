#include "CxxTypeName.h"

#include <sstream>

std::unique_ptr<CxxTypeName> CxxTypeName::getUnsolved()
{
	return std::make_unique<CxxTypeName>(L"unsolved-type");
}

std::unique_ptr<CxxTypeName> CxxTypeName::makeUnsolvedIfNull(std::unique_ptr<CxxTypeName> name)
{
	if (name)
	{
		return name;
	}

	return getUnsolved();
}

CxxTypeName::Modifier::Modifier(std::wstring symbol): symbol(std::move(symbol)) {}

CxxTypeName::CxxTypeName(std::wstring name): m_name(std::move(name)) {}

CxxTypeName::CxxTypeName(std::wstring name, std::vector<std::wstring> templateArguments)
	: m_name(std::move(name)), m_templateArguments(std::move(templateArguments))
{
}

CxxTypeName::CxxTypeName(
	std::wstring name, std::vector<std::wstring> templateArguments, std::shared_ptr<CxxName> parent)
	: CxxName(parent), m_name(std::move(name)), m_templateArguments(std::move(templateArguments))
{
}

NameHierarchy CxxTypeName::toNameHierarchy() const
{
	NameHierarchy ret = getParent() ? getParent()->toNameHierarchy()
									: NameHierarchy(NAME_DELIMITER_CXX);
	ret.push(m_name + getTemplateSuffix(m_templateArguments));
	return ret;
}

void CxxTypeName::addQualifier(const CxxQualifierFlags::QualifierType qualifier)
{
	if (m_modifiers.empty())
	{
		m_qualifierFlags.addQualifier(qualifier);
	}
	else
	{
		m_modifiers.back().qualifierFlags.addQualifier(qualifier);
	}
}

void CxxTypeName::addModifier(Modifier modifier)
{
	m_modifiers.emplace_back(std::move(modifier));
}

std::wstring CxxTypeName::toString() const
{
	std::wstringstream ss;
	if (!m_qualifierFlags.empty())
	{
		ss << m_qualifierFlags.toString() << L' ';
	}

	ss << toNameHierarchy().getQualifiedName();

	for (const Modifier& modifier: m_modifiers)
	{
		ss << L' ' << modifier.symbol;
		if (!modifier.qualifierFlags.empty())
		{
			ss << L' ' << modifier.qualifierFlags.toString();
		}
	}
	return ss.str();
}
