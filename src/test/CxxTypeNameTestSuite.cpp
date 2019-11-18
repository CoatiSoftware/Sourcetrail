#include "catch.hpp"

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#	include "CxxTypeName.h"

TEST_CASE("type name created with name has no qualifiers or modifiers")
{
	CxxTypeName typeName(L"int", std::vector<std::wstring>(), std::shared_ptr<CxxName>());
	REQUIRE(L"int" == typeName.toString());
}

TEST_CASE("type name created with name and const qualifier has no modifiers")
{
	CxxTypeName typeName(L"int", std::vector<std::wstring>(), std::shared_ptr<CxxName>());
	typeName.addQualifier(CxxQualifierFlags::QUALIFIER_CONST);
	REQUIRE(L"const int" == typeName.toString());
}

TEST_CASE("type name created with name and array modifier has array modifier")
{
	CxxTypeName typeName(L"int", std::vector<std::wstring>(), std::shared_ptr<CxxName>());
	typeName.addModifier(CxxTypeName::Modifier(L"[]"));
	REQUIRE(L"int []" == typeName.toString());
}

TEST_CASE("type name created with name and const pointer modifier has const pointer modifier")
{
	CxxTypeName typeName(L"int", std::vector<std::wstring>(), std::shared_ptr<CxxName>());
	typeName.addModifier(CxxTypeName::Modifier(L"*"));
	typeName.addQualifier(CxxQualifierFlags::QUALIFIER_CONST);
	REQUIRE(L"int * const" == typeName.toString());
}

TEST_CASE("type name created with name and pointer pointer modifier has pointer pointer modifier")
{
	CxxTypeName typeName(L"int", std::vector<std::wstring>(), std::shared_ptr<CxxName>());
	typeName.addModifier(CxxTypeName::Modifier(L"*"));
	typeName.addModifier(CxxTypeName::Modifier(L"*"));
	REQUIRE(L"int * *" == typeName.toString());
}

#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
