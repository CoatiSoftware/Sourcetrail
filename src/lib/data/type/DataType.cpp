#include "data/type/DataType.h"
#include "data/type/modifier/DataTypeModifierPointer.h"

DataType::DataType(const std::string& typeName)
	: m_typeName(typeName)
{
}

DataType::DataType(clang::QualType qualType)
{
	while (true)
	{
		const clang::Type* type = qualType.getTypePtr();
		if (type->isPointerType())
		{
			std::shared_ptr<DataTypeModifier> modifier = std::make_shared<DataTypeModifierPointer>();
			if (qualType.isConstQualified())
				modifier->addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
			m_modifierStack.push(modifier);

			qualType = type->getPointeeType();
		}
		else if (type->isStructureOrClassType() || type->isEnumeralType())
		{
			m_typeName = qualType.getAsString();

			// we are working on the string here to not lose the namespace information stored in the name.
			m_typeName = m_typeName.substr(m_typeName.find(' ') + 1, m_typeName.size() - 1);

			//m_typeName = qualType.getBaseTypeIdentifier()->getName(); // this one does not keep namespace information.
			break;
		}
		else
		{
			m_typeName = qualType.getUnqualifiedType().getAsString();
			break;
		}
	}

	if (qualType.isConstQualified())
		m_qualifierList.addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
}

DataType::~DataType()
{
}

std::string DataType::getFullTypeName() const
{
	return m_modifierStack.applyTo(m_qualifierList.applyTo(m_typeName));
}

std::string DataType::getRawTypeName() const
{
	return m_typeName;
}
