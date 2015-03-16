#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"

#include "data/type/DataType.h"
#include "data/type/modifier/DataTypeModifierArray.h"
#include "data/type/modifier/DataTypeModifierPointer.h"
#include "data/type/modifier/DataTypeModifierReference.h"

class DataTypeTestSuite: public CxxTest::TestSuite
{
public:
	void test_datatype_created_with_name_has_no_qualifiers_or_modifiers()
	{
		DataType dataType(utility::splitToVector("int", "::"));
		TS_ASSERT_EQUALS("int", dataType.getFullTypeName());
	}

	void test_datatype_created_with_name_and_empty_qualifiers_has_no_qualifiers_or_modifiers()
	{
		DataTypeQualifierList qualifierList;
		DataType dataType(utility::splitToVector("int", "::"), qualifierList);
		TS_ASSERT_EQUALS("int", dataType.getFullTypeName());
	}

	void test_datatype_created_with_name_and_const_qualifier_has_no_modifiers()
	{
		DataTypeQualifierList qualifierList;
		qualifierList.addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
		DataType dataType(utility::splitToVector("int", "::"), qualifierList);
		TS_ASSERT_EQUALS("int const", dataType.getFullTypeName());
	}

	void test_datatype_created_with_name_and_empty_modifierstack_has_no_modifiers()
	{
		DataTypeQualifierList qualifierList;
		DataTypeModifierStack modifierStack;
		DataType dataType(utility::splitToVector("int", "::"), qualifierList, modifierStack);
		TS_ASSERT_EQUALS("int", dataType.getFullTypeName());
	}

	void test_datatype_created_with_name_and_array_modifier_has_array_modifier()
	{
		DataTypeQualifierList qualifierList;
		DataTypeModifierStack modifierStack;
		modifierStack.push(std::make_shared<DataTypeModifierArray>());
		DataType dataType(utility::splitToVector("int", "::"), qualifierList, modifierStack);
		TS_ASSERT_EQUALS("int []", dataType.getFullTypeName());
	}

	void test_datatype_created_with_name_and_pointer_modifier_has_pointer_modifier()
	{
		DataTypeQualifierList qualifierList;
		DataTypeModifierStack modifierStack;
		modifierStack.push(std::make_shared<DataTypeModifierPointer>());
		DataType dataType(utility::splitToVector("int", "::"), qualifierList, modifierStack);
		TS_ASSERT_EQUALS("int *", dataType.getFullTypeName());
	}

	void test_datatype_created_with_name_and_reference_modifier_has_reference_modifier()
	{
		DataTypeQualifierList qualifierList;
		DataTypeModifierStack modifierStack;
		modifierStack.push(std::make_shared<DataTypeModifierReference>());
		DataType dataType(utility::splitToVector("int", "::"), qualifierList, modifierStack);
		TS_ASSERT_EQUALS("int &", dataType.getFullTypeName());
	}

	void test_datatype_created_with_name_and_const_pointer_modifier_has_const_pointer_modifier()
	{
		DataTypeQualifierList qualifierList;
		DataTypeModifierStack modifierStack;
		std::shared_ptr<DataTypeModifier> modifier = std::make_shared<DataTypeModifierPointer>();
		modifier->addQualifier(DataTypeQualifierList::QUALIFIER_CONST);
		modifierStack.push(modifier);
		DataType dataType(utility::splitToVector("int", "::"), qualifierList, modifierStack);
		TS_ASSERT_EQUALS("int * const", dataType.getFullTypeName());
	}

	void test_datatype_created_with_name_and_pointer_pointer_modifier_has_pointer_pointer_modifier()
	{
		DataTypeQualifierList qualifierList;
		DataTypeModifierStack modifierStack;
		modifierStack.push(std::make_shared<DataTypeModifierPointer>());
		modifierStack.push(std::make_shared<DataTypeModifierPointer>());
		DataType dataType(utility::splitToVector("int", "::"), qualifierList, modifierStack);
		TS_ASSERT_EQUALS("int * *", dataType.getFullTypeName());
	}
};
