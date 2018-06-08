#include "utility/codeblocks/CodeblocksUnit.h"

#include "tinyxml/tinyxml.h"

#include "utility/utilityString.h"

namespace Codeblocks
{
	std::string Unit::getXmlElementName()
	{
		return "Unit";
	}

	std::shared_ptr<Unit> Unit::create(const TiXmlElement* element)
	{
		if (!element || element->Value() != getXmlElementName())
		{
			return std::shared_ptr<Unit>();
		}

		std::shared_ptr<Unit> unit(new Unit());

		{
			const char* value = element->Attribute("filename");
			if (!value)
			{
				return std::shared_ptr<Unit>();
			}
			unit->m_filename = utility::decodeFromUtf8(value);
		}

		const TiXmlElement* optionElement = element->FirstChildElement("Option");
		while (optionElement)
		{
			{
				const char* value = optionElement->Attribute("compilerVar");
				if (value)
				{
					unit->m_compilerVar = stringToCompilerVarType(value);
				}
			}
			{
				int value = 0;
				if (optionElement->QueryIntAttribute("compile", &value) == TIXML_SUCCESS)
				{
					unit->m_compile = (value == 1);
				}
			}
			{
				const char* value = optionElement->Attribute("target");
				if (value)
				{
					unit->m_targetNames.insert(utility::decodeFromUtf8(value));
				}
			}

			optionElement = optionElement->NextSiblingElement("Option");
		}

		return unit;
	}

	std::wstring Unit::getFilename() const
	{
		return m_filename;
	}

	CompilerVarType Unit::getCompilerVar() const
	{
		return m_compilerVar;
	}

	bool Unit::getCompile() const
	{
		return m_compile;
	}

	std::set<std::wstring> Unit::getTargetNames() const
	{
		return m_targetNames;
	}

	Unit::Unit()
		: m_filename()
		, m_compilerVar(COMPILER_VAR_CPP)
		, m_compile(true)
	{
	}
}
