#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"

#include "data/type/DataType.h"
#include "data/type/NamedDataType.h"
#include "data/type/ArrayModifiedDataType.h"
#include "data/type/PointerModifiedDataType.h"
#include "data/type/ReferenceModifiedDataType.h"

class DataTypeTestSuite: public CxxTest::TestSuite
{
public:
	void test_datatype_created_with_name_has_no_qualifiers_or_modifiers()
	{
		NamedDataType dataType(utility::splitToVector("int", "::"));
		TS_ASSERT_EQUALS("int", dataType.getFullTypeName());
	}

	void test_datatype_created_with_name_and_const_qualifier_has_no_modifiers()
	{
		NamedDataType dataType(utility::splitToVector("int", "::"));
		dataType.addQualifier(DataType::QUALIFIER_CONST);
		TS_ASSERT_EQUALS("int const", dataType.getFullTypeName());
	}

	//void __test_datatype_created_with_name_and_empty_modifierstack_has_no_modifiers()
	//{
	//	DataTypeQualifierList qualifierList;
	//	DataTypeModifierStack modifierStack;
	//	DataType dataType(utility::splitToVector("int", "::"), qualifierList, modifierStack);
	//	TS_ASSERT_EQUALS("int", dataType.getFullTypeName());
	//}

	void test_datatype_created_with_name_and_array_modifier_has_array_modifier()
	{
		std::shared_ptr<DataType> dataType1 = std::make_shared<NamedDataType>(utility::splitToVector("int", "::"));
		std::shared_ptr<DataType> dataType2 = std::make_shared<ArrayModifiedDataType>(dataType1);
		TS_ASSERT_EQUALS("int []", dataType2->getFullTypeName());
	}

	void test_datatype_created_with_name_and_pointer_modifier_has_pointer_modifier()
	{
		std::shared_ptr<DataType> dataType1 = std::make_shared<NamedDataType>(utility::splitToVector("int", "::"));
		std::shared_ptr<DataType> dataType2 = std::make_shared<PointerModifiedDataType>(dataType1);
		TS_ASSERT_EQUALS("int *", dataType2->getFullTypeName());
	}

	void test_datatype_created_with_name_and_reference_modifier_has_reference_modifier()
	{
		std::shared_ptr<DataType> dataType1 = std::make_shared<NamedDataType>(utility::splitToVector("int", "::"));
		std::shared_ptr<DataType> dataType2 = std::make_shared<ReferenceModifiedDataType>(dataType1);
		TS_ASSERT_EQUALS("int &", dataType2->getFullTypeName());
	}

	void test_datatype_created_with_name_and_const_pointer_modifier_has_const_pointer_modifier()
	{
		std::shared_ptr<DataType> dataType1 = std::make_shared<NamedDataType>(utility::splitToVector("int", "::"));
		std::shared_ptr<DataType> dataType2 = std::make_shared<PointerModifiedDataType>(dataType1);
		dataType2->addQualifier(DataType::QUALIFIER_CONST);
		TS_ASSERT_EQUALS("int * const", dataType2->getFullTypeName());
	}

	void test_datatype_created_with_name_and_pointer_pointer_modifier_has_pointer_pointer_modifier()
	{
		std::shared_ptr<DataType> dataType1 = std::make_shared<NamedDataType>(utility::splitToVector("int", "::"));
		std::shared_ptr<DataType> dataType2 = std::make_shared<PointerModifiedDataType>(dataType1);
		std::shared_ptr<DataType> dataType3 = std::make_shared<PointerModifiedDataType>(dataType2);
		TS_ASSERT_EQUALS("int * *", dataType3->getFullTypeName());
	}
};
