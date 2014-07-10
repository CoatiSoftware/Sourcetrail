#include "data/parser/cxx/utilityCxx.h"

#include "data/type/DataType.h"
#include "data/type/modifier/DataTypeModifierPointer.h"
#include "data/type/DataTypeModifierStack.h"
#include "data/type/DataTypeQualifierList.h"

namespace utility
{
	DataType qualTypeToDataType(clang::QualType qualType)
	{
		std::string typeName;
		DataTypeQualifierList qualifierList;
		DataTypeModifierStack modifierStack;

		while (true)
		{
			const clang::Type* type = qualType.getTypePtr();
			if (type->isPointerType())
			{
				std::shared_ptr<DataTypeModifier> modifier = std::make_shared<DataTypeModifierPointer>();
				if (qualType.isConstQualified())
					modifier->addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
				modifierStack.push(modifier);

				qualType = type->getPointeeType();
			}
			else if (type->isStructureOrClassType() || type->isEnumeralType())
			{
				typeName = qualType.getAsString();

				// we are working on the string here to not lose the namespace information stored in the name.
				size_t nameStartPosition = typeName.find(' ');
				if (nameStartPosition != typeName.npos)
				{
					nameStartPosition += 1;
				}
				else
				{
					nameStartPosition = 0;
				}
				typeName = typeName.substr(nameStartPosition, typeName.size());

				//m_typeName = qualType.getBaseTypeIdentifier()->getName(); // this one does not keep namespace information.
				break;
			}
			else
			{
				typeName = qualType.getUnqualifiedType().getAsString();
				break;
			}
		}

		if (qualType.isConstQualified())
			qualifierList.addQualifier(DataTypeQualifierList::QUALIFIER_CONST);

		return DataType(typeName, qualifierList, modifierStack);
	}
}
