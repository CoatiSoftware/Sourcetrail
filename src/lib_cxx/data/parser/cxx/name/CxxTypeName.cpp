#include "data/parser/cxx/name/CxxTypeName.h"

CxxTypeName::Modifier::Modifier(std::string symbol)
	: symbol(symbol)
{
}

CxxTypeName::CxxTypeName(std::string name, std::vector<std::string> templateArguments)
	: m_name(name)
	, m_templateArguments(templateArguments)
{
}

CxxTypeName::CxxTypeName(
	std::string name,
	std::vector<std::string> templateArguments,
	std::shared_ptr<CxxName> parent
)
	: CxxName(parent)
	, m_name(name)
	, m_templateArguments(templateArguments)
{
}

CxxTypeName::~CxxTypeName()
{
}

NameHierarchy CxxTypeName::toNameHierarchy() const
{
	NameHierarchy ret = getParent() ? getParent()->toNameHierarchy(): NameHierarchy();
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

std::string CxxTypeName::toString() const
{
	std::string ret = "";
	if (!m_qualifierFlags.empty())
	{
		ret += m_qualifierFlags.toString() + " ";
	}
	ret += toNameHierarchy().getQualifiedName();

	for (Modifier modifier: m_modifiers)
	{
		ret += " " + modifier.symbol;
		if (!modifier.qualifierFlags.empty())
		{
			ret += " " + modifier.qualifierFlags.toString();
		}
	}
	return ret;
}

std::string CxxTypeName::getTypeNameString() const
{
	std::string ret = m_name;
	if (!m_templateArguments.empty())
	{
		ret += "<";
		for (size_t i = 0; i < m_templateArguments.size(); i++)
		{
			if (i != 0)
			{
				ret += ", ";
			}
			ret += m_templateArguments[i];
		}
		ret += ">";
	}
	return ret;
}
