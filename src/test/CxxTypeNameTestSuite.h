#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"
#include "data/parser/cxx/name/CxxTypeName.h"

class CxxTypeNameTestSuite: public CxxTest::TestSuite
{
public:
	void test_type_name_created_with_name_has_no_qualifiers_or_modifiers()
	{
		CxxTypeName typeName("int", std::vector<std::string>(), std::shared_ptr<CxxName>());
		TS_ASSERT_EQUALS("int", typeName.toString());
	}

	void test_type_name_created_with_name_and_const_qualifier_has_no_modifiers()
	{
		CxxTypeName typeName("int", std::vector<std::string>(), std::shared_ptr<CxxName>());
		typeName.addQualifier(CxxQualifierFlags::QUALIFIER_CONST);
		TS_ASSERT_EQUALS("const int", typeName.toString());
	}

	void test_type_name_created_with_name_and_array_modifier_has_array_modifier()
	{
		CxxTypeName typeName("int", std::vector<std::string>(), std::shared_ptr<CxxName>());
		typeName.addModifier(CxxTypeName::Modifier("[]"));
		TS_ASSERT_EQUALS("int []", typeName.toString());
	}

	void test_type_name_created_with_name_and_const_pointer_modifier_has_const_pointer_modifier()
	{
		CxxTypeName typeName("int", std::vector<std::string>(), std::shared_ptr<CxxName>());
		typeName.addModifier(CxxTypeName::Modifier("*"));
		typeName.addQualifier(CxxQualifierFlags::QUALIFIER_CONST);
		TS_ASSERT_EQUALS("int * const", typeName.toString());
	}

	void test_type_name_created_with_name_and_pointer_pointer_modifier_has_pointer_pointer_modifier()
	{
		CxxTypeName typeName("int", std::vector<std::string>(), std::shared_ptr<CxxName>());
		typeName.addModifier(CxxTypeName::Modifier("*"));
		typeName.addModifier(CxxTypeName::Modifier("*"));
		TS_ASSERT_EQUALS("int * *", typeName.toString());
	}
};
