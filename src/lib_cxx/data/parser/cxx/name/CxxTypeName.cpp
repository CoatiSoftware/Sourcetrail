#include "CxxTypeName.h"

std::shared_ptr<CxxTypeName> CxxTypeName::makeUnsolvedIfNull(std::shared_ptr<CxxTypeName> name)
{
	if (name)
	{
		return name;
	}
	return std::make_shared<CxxTypeName>(
		L"unsolved-type", std::vector<std::wstring>()
	);
}

CxxTypeName::Modifier::Modifier(std::wstring&& symbol)
	: symbol(std::move(symbol))
{
}

//CxxTypeName::CxxTypeName(const std::wstring& name, const std::vector<std::wstring>& templateArguments)
//	: m_name(name)
//	, m_templateArguments(templateArguments)
//{
//}

CxxTypeName::CxxTypeName(std::wstring&& name, std::vector<std::wstring>&& templateArguments)
	: m_name(std::move(name))
	, m_templateArguments(std::move(templateArguments))
{
}

//CxxTypeName::CxxTypeName(
//	const std::wstring& name,
//	const std::vector<std::wstring>& templateArguments,
//	std::shared_ptr<CxxName> parent
//)
//	: CxxName(parent)
//	, m_name(name)
//	, m_templateArguments(templateArguments)
//{
//}

CxxTypeName::CxxTypeName(
	std::wstring&& name,
	std::vector<std::wstring>&& templateArguments,
	std::shared_ptr<CxxName> parent
)
	: CxxName(parent)
	, m_name(std::move(name))
	, m_templateArguments(std::move(templateArguments))
{
}

NameHierarchy CxxTypeName::toNameHierarchy() const
{
	NameHierarchy ret = getParent() ? getParent()->toNameHierarchy(): NameHierarchy(NAME_DELIMITER_CXX);
	ret.push(std::make_shared<NameElement>(getTypeNameString()));
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

void CxxTypeName::addModifier(const Modifier& modifier)
{
	m_modifiers.push_back(modifier);
}

std::wstring CxxTypeName::toString() const
{
	std::wstring ret = L"";
	if (!m_qualifierFlags.empty())
	{
		ret += m_qualifierFlags.toString() + L" ";
	}
	ret += toNameHierarchy().getQualifiedName();

	for (const Modifier& modifier: m_modifiers)
	{
		ret += L" " + modifier.symbol;
		if (!modifier.qualifierFlags.empty())
		{
			ret += L" " + modifier.qualifierFlags.toString();
		}
	}
	return ret;
}

std::wstring CxxTypeName::getTypeNameString() const
{
	std::wstring ret = m_name;
	if (!m_templateArguments.empty())
	{
		ret += L"<";
		for (size_t i = 0; i < m_templateArguments.size(); i++)
		{
			if (i != 0)
			{
				ret += L", ";
			}
			ret += m_templateArguments[i];
		}
		ret += L">";
	}
	return ret;
}
