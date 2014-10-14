#include "data/parser/cxx/utilityCxx.h"

#include "data/type/DataType.h"
#include "data/type/modifier/DataTypeModifierArray.h"
#include "data/type/modifier/DataTypeModifierPointer.h"
#include "data/type/modifier/DataTypeModifierReference.h"
#include "data/type/DataTypeModifierStack.h"
#include "data/type/DataTypeQualifierList.h"
#include "utility/utilityString.h"

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
			if (type->getAs<clang::TypedefType>())
			{
				typeName = utility::substrAfter(qualType.getAsString(), ' ');
				break;
			}
			else if (type->isPointerType())
			{
				std::shared_ptr<DataTypeModifier> modifier = std::make_shared<DataTypeModifierPointer>();
				if (qualType.isConstQualified())
				{
					modifier->addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
				}
				modifierStack.push(modifier);

				qualType = type->getPointeeType();
			}
			else if (type->isArrayType())
			{
				std::shared_ptr<DataTypeModifier> modifier = std::make_shared<DataTypeModifierArray>();
				if (qualType.isConstQualified())
				{
					modifier->addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
				}
				modifierStack.push(modifier);

				qualType = clang::dyn_cast<clang::ArrayType>(type)->getElementType();
			}
			else if (type->isReferenceType())
			{
				std::shared_ptr<DataTypeModifier> modifier = std::make_shared<DataTypeModifierReference>();
				// references can not be const qualified
				modifierStack.push(modifier);

				qualType = type->getPointeeType();
			}
			else if (type->isStructureOrClassType() || type->isEnumeralType())
			{
				// we are working on the string here to not lose the namespace information stored in the name.
				typeName = utility::substrAfter(qualType.getAsString(), ' ');

				// typeName = qualType.getBaseTypeIdentifier()->getName(); // this one does not keep namespace information.
				break;
			}
			else
			{
				typeName = qualType.getUnqualifiedType().getAsString();
				break;
			}
		}

		if (qualType.isConstQualified())
		{
			qualifierList.addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
		}

		if (typeName == "_Bool")
		{
			typeName = "bool";
		}

		return DataType(typeName, qualifierList, modifierStack);
	}
}
