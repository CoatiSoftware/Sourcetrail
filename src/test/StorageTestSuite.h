#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"

#include "data/graph/token_component/TokenComponentAbstraction.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/graph/token_component/TokenComponentSignature.h"
#include "data/graph/token_component/TokenComponentStatic.h"
#include "data/location/TokenLocation.h"
#include "data/parser/ParseFunction.h"
#include "data/parser/ParseLocation.h"
#include "data/parser/ParseTypeUsage.h"
#include "data/parser/ParseVariable.h"
#include "data/Storage.h"
#include "data/type/DataType.h"

class StorageTestSuite: public CxxTest::TestSuite
{
public:
	void setUp()
	{
		m_filePath = "file.cpp";
	}

	void test_storage_saves_typedef()
	{
		TestStorage storage;
		Id id = storage.onTypedefParsed(validLocation(1), utility::splitToVector("type", "::"), typeUsage("int"), ParserClient::ACCESS_NONE);

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "type");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_TYPEDEF);

		Edge* typeEdge = node->findEdgeOfType(Edge::EDGE_TYPEDEF_OF);
		TS_ASSERT(typeEdge);
		TS_ASSERT_EQUALS(typeEdge->getTo()->getFullName(), "int");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 1));
	}

	void test_storage_saves_class()
	{
		TestStorage storage;
		Id id = storage.onClassParsed(validLocation(1), utility::splitToVector("Class", "::"), ParserClient::ACCESS_NONE, validLocation(2));

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "Class");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_CLASS);

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 2);
		TS_ASSERT(isValidLocation(locations[0], 1));
		TS_ASSERT(isValidLocation(locations[1], 2));
		TS_ASSERT_EQUALS(locations[1]->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_struct()
	{
		TestStorage storage;
		Id id = storage.onStructParsed(validLocation(1), utility::splitToVector("Struct", "::"), ParserClient::ACCESS_NONE, validLocation(2));

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "Struct");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_STRUCT);

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 2);
		TS_ASSERT(isValidLocation(locations[0], 1));
		TS_ASSERT(isValidLocation(locations[1], 2));
		TS_ASSERT_EQUALS(locations[1]->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_global_variable()
	{
		TestStorage storage;
		Id id = storage.onGlobalVariableParsed(validLocation(42), ParseVariable(typeUsage("char"), utility::splitToVector("Global", "::"), false));

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "Global");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_GLOBAL_VARIABLE);
		TS_ASSERT(!node->getComponent<TokenComponentStatic>());

		Edge* typeEdge = node->findEdgeOfType(Edge::EDGE_TYPE_OF);
		TS_ASSERT(typeEdge);
		TS_ASSERT_EQUALS(typeEdge->getTo()->getFullName(), "char");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 42));
	}

	void test_storage_saves_global_variable_static()
	{
		TestStorage storage;
		Id id = storage.onGlobalVariableParsed(validLocation(7), ParseVariable(typeUsage("char"), utility::splitToVector("Global", "::"), true));

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "Global");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_GLOBAL_VARIABLE);
		TS_ASSERT(node->getComponent<TokenComponentStatic>());

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 7));
	}

	void test_storage_saves_field()
	{
		TestStorage storage;
		Id id = storage.onFieldParsed(
			validLocation(3), ParseVariable(typeUsage("bool"), utility::splitToVector("m_field", "::"), false), ParserClient::ACCESS_NONE
		);

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "m_field");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_FIELD);

		Edge* typeEdge = node->findEdgeOfType(Edge::EDGE_TYPE_OF);
		TS_ASSERT(typeEdge);
		TS_ASSERT_EQUALS(typeEdge->getTo()->getFullName(), "bool");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 3));
	}

	void test_storage_saves_field_as_member()
	{
		TestStorage storage;
		Id id = storage.onFieldParsed(
			validLocation(11), ParseVariable(typeUsage("bool"), utility::splitToVector("Struct::m_field", "::"), false), ParserClient::ACCESS_PUBLIC
		);

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getName(), "m_field");
		TS_ASSERT_EQUALS(node->getFullName(), "Struct::m_field");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_FIELD);

		Edge* memberEdge = node->getMemberEdge();
		TS_ASSERT(memberEdge);
		TS_ASSERT_EQUALS(memberEdge->getType(), Edge::EDGE_MEMBER);

		TS_ASSERT(memberEdge->getComponent<TokenComponentAccess>());
		TS_ASSERT_EQUALS(
			memberEdge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PUBLIC
		);

		TS_ASSERT_EQUALS(memberEdge->getFrom()->getFullName(), "Struct");
		TS_ASSERT_EQUALS(memberEdge->getFrom()->getType(), Node::NODE_UNDEFINED);

		Edge* typeEdge = node->findEdgeOfType(Edge::EDGE_TYPE_OF);
		TS_ASSERT(typeEdge);
		TS_ASSERT_EQUALS(typeEdge->getTo()->getFullName(), "bool");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 11));
	}

	void test_storage_saves_function()
	{
		TestStorage storage;
		Id id = storage.onFunctionParsed(
			validLocation(14), ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char")), validLocation(41)
		);

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "isTrue");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_FUNCTION);

		Edge* returnEdge = node->findEdgeOfType(Edge::EDGE_RETURN_TYPE_OF);
		TS_ASSERT(returnEdge);
		TS_ASSERT_EQUALS(returnEdge->getTo()->getFullName(), "bool");

		Edge* paramEdge = node->findEdgeOfType(Edge::EDGE_PARAMETER_TYPE_OF);
		TS_ASSERT(paramEdge);
		TS_ASSERT_EQUALS(paramEdge->getTo()->getFullName(), "char");

		TS_ASSERT(node->getComponent<TokenComponentSignature>());
		TS_ASSERT_EQUALS(storage.getWord(node->getComponent<TokenComponentSignature>()->getWordId()), "isTrue(char)");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 2);
		TS_ASSERT(isValidLocation(locations[0], 14));
		TS_ASSERT(isValidLocation(locations[1], 41));
		TS_ASSERT_EQUALS(locations[1]->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_method()
	{
		TestStorage storage;
		Id id = storage.onMethodParsed(
			validLocation(9),
			ParseFunction(typeUsage("void"), utility::splitToVector("isMethod", "::"), parameters("bool")),
			ParserClient::ACCESS_NONE,
			ParserClient::ABSTRACTION_NONE,
			validLocation(4)
		);

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "isMethod");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_METHOD);

		Edge* returnEdge = node->findEdgeOfType(Edge::EDGE_RETURN_TYPE_OF);
		TS_ASSERT(returnEdge);
		TS_ASSERT_EQUALS(returnEdge->getTo()->getFullName(), "void");

		Edge* paramEdge = node->findEdgeOfType(Edge::EDGE_PARAMETER_TYPE_OF);
		TS_ASSERT(paramEdge);
		TS_ASSERT_EQUALS(paramEdge->getTo()->getFullName(), "bool");

		TS_ASSERT(node->getComponent<TokenComponentSignature>());
		TS_ASSERT_EQUALS(storage.getWord(node->getComponent<TokenComponentSignature>()->getWordId()), "isMethod(bool)");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 2);
		TS_ASSERT(isValidLocation(locations[0], 9));
		TS_ASSERT(isValidLocation(locations[1], 4));
		TS_ASSERT_EQUALS(locations[1]->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_method_static()
	{
		TestStorage storage;
		Id id = storage.onMethodParsed(
			validLocation(1),
			ParseFunction(typeUsage("void"), utility::splitToVector("isMethod", "::"), parameters("bool"), true),
			ParserClient::ACCESS_NONE,
			ParserClient::ABSTRACTION_NONE,
			validLocation(4)
		);

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "isMethod");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_METHOD);
		TS_ASSERT(node->getComponent<TokenComponentStatic>());
	}

	void test_storage_saves_method_as_member()
	{
		TestStorage storage;
		Id id = storage.onMethodParsed(
			validLocation(1),
			ParseFunction(typeUsage("void"), utility::splitToVector("Class::isMethod", "::"), parameters("bool")),
			ParserClient::ACCESS_PROTECTED,
			ParserClient::ABSTRACTION_VIRTUAL,
			validLocation(4)
		);

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getName(), "isMethod");
		TS_ASSERT_EQUALS(node->getFullName(), "Class::isMethod");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_METHOD);

		TS_ASSERT(node->getComponent<TokenComponentAbstraction>());
		TS_ASSERT_EQUALS(
			node->getComponent<TokenComponentAbstraction>()->getAbstraction(),
			TokenComponentAbstraction::ABSTRACTION_VIRTUAL
		);

		Edge* memberEdge = node->getMemberEdge();
		TS_ASSERT(memberEdge);
		TS_ASSERT_EQUALS(memberEdge->getType(), Edge::EDGE_MEMBER);

		TS_ASSERT(memberEdge->getComponent<TokenComponentAccess>());
		TS_ASSERT_EQUALS(
			memberEdge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PROTECTED
		);

		TS_ASSERT_EQUALS(memberEdge->getFrom()->getFullName(), "Class");
		TS_ASSERT_EQUALS(memberEdge->getFrom()->getType(), Node::NODE_UNDEFINED);
	}

	void test_storage_saves_namespace()
	{
		TestStorage storage;
		Id id = storage.onNamespaceParsed(validLocation(1), utility::splitToVector("utility", "::"), validLocation(2));

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "utility");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_NAMESPACE);

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 2);
		TS_ASSERT(isValidLocation(locations[0], 1));
		TS_ASSERT(isValidLocation(locations[1], 2));
		TS_ASSERT_EQUALS(locations[1]->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_enum()
	{
		TestStorage storage;
		Id id = storage.onEnumParsed(validLocation(17), utility::splitToVector("Category", "::"), ParserClient::ACCESS_NONE, validLocation(23));

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "Category");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_ENUM);

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 2);
		TS_ASSERT(isValidLocation(locations[0], 17));
		TS_ASSERT(isValidLocation(locations[1], 23));
		TS_ASSERT_EQUALS(locations[1]->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_enum_as_member()
	{
		TestStorage storage;
		Id id =
			storage.onEnumParsed(validLocation(1), utility::splitToVector("Class::Category", "::"),
			ParserClient::ACCESS_PRIVATE, validLocation(2)
		);

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "Class::Category");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_ENUM);

		Edge* memberEdge = node->getMemberEdge();
		TS_ASSERT(memberEdge);
		TS_ASSERT_EQUALS(memberEdge->getType(), Edge::EDGE_MEMBER);

		TS_ASSERT(memberEdge->getComponent<TokenComponentAccess>());
		TS_ASSERT_EQUALS(
			memberEdge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PRIVATE
		);

		TS_ASSERT_EQUALS(memberEdge->getFrom()->getFullName(), "Class");
		TS_ASSERT_EQUALS(memberEdge->getFrom()->getType(), Node::NODE_UNDEFINED);
	}

	void test_storage_saves_enum_constant()
	{
		TestStorage storage;
		Id id = storage.onEnumConstantParsed(validLocation(1), utility::splitToVector("VALUE", "::"));

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "VALUE");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_ENUM_CONSTANT);

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 1));
	}

	void test_storage_saves_class_inheritance()
	{
		TestStorage storage;
		storage.onClassParsed(validLocation(), utility::splitToVector("ClassA", "::"), ParserClient::ACCESS_NONE, validLocation());
		storage.onClassParsed(validLocation(), utility::splitToVector("ClassB", "::"), ParserClient::ACCESS_NONE, validLocation());
		Id id =
			storage.onInheritanceParsed(validLocation(5), utility::splitToVector("ClassB", "::"),
			utility::splitToVector("ClassA", "::"), ParserClient::ACCESS_PUBLIC
		);

		Edge* edge = storage.getEdgeWithId(id);
		TS_ASSERT(edge);
		TS_ASSERT_EQUALS(edge->getType(), Edge::EDGE_INHERITANCE);

		TS_ASSERT(edge->getComponent<TokenComponentAccess>());
		TS_ASSERT_EQUALS(edge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PUBLIC);

		TS_ASSERT_EQUALS(edge->getFrom()->getFullName(), "ClassB");
		TS_ASSERT_EQUALS(edge->getTo()->getFullName(), "ClassA");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 5));
	}

	void test_storage_saves_struct_inheritance()
	{
		TestStorage storage;
		storage.onStructParsed(validLocation(), utility::splitToVector("StructA", "::"), ParserClient::ACCESS_NONE, validLocation());
		storage.onStructParsed(validLocation(), utility::splitToVector("StructB", "::"), ParserClient::ACCESS_NONE, validLocation());
		Id id =
			storage.onInheritanceParsed(validLocation(5), utility::splitToVector("StructB", "::"),
			utility::splitToVector("StructA", "::"), ParserClient::ACCESS_PUBLIC
		);

		Edge* edge = storage.getEdgeWithId(id);
		TS_ASSERT(edge);
		TS_ASSERT_EQUALS(edge->getType(), Edge::EDGE_INHERITANCE);

		TS_ASSERT(edge->getComponent<TokenComponentAccess>());
		TS_ASSERT_EQUALS(edge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PUBLIC);

		TS_ASSERT_EQUALS(edge->getFrom()->getFullName(), "StructB");
		TS_ASSERT_EQUALS(edge->getTo()->getFullName(), "StructA");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 5));
	}

	void test_storage_saves_method_override()
	{
		TestStorage storage;

		ParseFunction a(typeUsage("void"), utility::splitToVector("A::isMethod", "::"), parameters("bool"));
		ParseFunction b(typeUsage("void"), utility::splitToVector("B::isMethod", "::"), parameters("bool"));

		storage.onMethodParsed(validLocation(9), a, ParserClient::ACCESS_PRIVATE, ParserClient::ABSTRACTION_VIRTUAL, validLocation(4));
		storage.onMethodParsed(validLocation(7), b, ParserClient::ACCESS_PRIVATE, ParserClient::ABSTRACTION_NONE, validLocation(3));

		Id id = storage.onMethodOverrideParsed(a, b);

		Edge* edge = storage.getEdgeWithId(id);
		TS_ASSERT(edge);
		TS_ASSERT_EQUALS(edge->getType(), Edge::EDGE_OVERRIDE);
		TS_ASSERT_EQUALS(edge->getFrom()->getFullName(), "A::isMethod");
		TS_ASSERT_EQUALS(edge->getTo()->getFullName(), "B::isMethod");
	}

	void test_storage_saves_call()
	{
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char")), validLocation()
		);
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("void"), utility::splitToVector("func", "::"), parameters("bool")), validLocation()
		);
		Id id = storage.onCallParsed(
			validLocation(9),
			ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char")),
			ParseFunction(typeUsage("void"), utility::splitToVector("func", "::"), parameters("bool"))
		);

		Edge* edge = storage.getEdgeWithId(id);
		TS_ASSERT(edge);
		TS_ASSERT_EQUALS(edge->getType(), Edge::EDGE_CALL);

		TS_ASSERT_EQUALS(edge->getFrom()->getFullName(), "isTrue");
		TS_ASSERT_EQUALS(edge->getTo()->getFullName(), "func");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 9));
	}

	void test_storage_saves_call_in_global_variable_declaration()
	{
		TestStorage storage;
		storage.onGlobalVariableParsed(validLocation(), ParseVariable(typeUsage("bool"), utility::splitToVector("global", "::"), false));
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char")), validLocation()
		);

		Id id = storage.onCallParsed(
			validLocation(7),
			ParseVariable(typeUsage("bool"), utility::splitToVector("global", "::"), false),
			ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char"))
		);

		Edge* edge = storage.getEdgeWithId(id);
		TS_ASSERT(edge);
		TS_ASSERT_EQUALS(edge->getType(), Edge::EDGE_CALL);

		TS_ASSERT_EQUALS(edge->getFrom()->getFullName(), "global");
		TS_ASSERT_EQUALS(edge->getTo()->getFullName(), "isTrue");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 7));
	}

	void test_storage_saves_field_usage()
	{
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char")), validLocation()
		);
		storage.onFieldParsed(
			validLocation(), ParseVariable(typeUsage("bool"), utility::splitToVector("Foo::m_field", "::"), false), ParserClient::ACCESS_PRIVATE
		);

		Id id = storage.onFieldUsageParsed(
			validLocation(7),
			ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char")),
			utility::splitToVector("Foo::m_field", "::")
		);

		Edge* edge = storage.getEdgeWithId(id);
		TS_ASSERT(edge);
		TS_ASSERT_EQUALS(edge->getType(), Edge::EDGE_USAGE);

		TS_ASSERT_EQUALS(edge->getFrom()->getFullName(), "isTrue");
		TS_ASSERT_EQUALS(edge->getTo()->getFullName(), "Foo::m_field");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 7));
	}

	void test_storage_saves_global_variable_usage()
	{
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char")), validLocation()
		);
		storage.onGlobalVariableParsed(validLocation(), ParseVariable(typeUsage("bool"), utility::splitToVector("global", "::"), false));

		Id id = storage.onGlobalVariableUsageParsed(
			validLocation(7),
			ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char")),
			utility::splitToVector("global", "::")
		);

		Edge* edge = storage.getEdgeWithId(id);
		TS_ASSERT(edge);
		TS_ASSERT_EQUALS(edge->getType(), Edge::EDGE_USAGE);

		TS_ASSERT_EQUALS(edge->getFrom()->getFullName(), "isTrue");
		TS_ASSERT_EQUALS(edge->getTo()->getFullName(), "global");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 7));
	}

	void test_storage_saves_type_usage()
	{
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"),
			parameters("char")), validLocation()
		);
		storage.onStructParsed(
			validLocation(), utility::splitToVector("Struct", "::"), ParserClient::ACCESS_NONE, validLocation());

		Id id = storage.onTypeUsageParsed(
			typeUsage("Struct"),
			ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char"))
		);

		Edge* edge = storage.getEdgeWithId(id);
		TS_ASSERT(edge);
		TS_ASSERT_EQUALS(edge->getType(), Edge::EDGE_TYPE_USAGE);

		TS_ASSERT_EQUALS(edge->getFrom()->getFullName(), "isTrue");
		TS_ASSERT_EQUALS(edge->getTo()->getFullName(), "Struct");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 0));
	}

	void test_storage_clears_single_file_data_of_single_file_storage()
	{
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"),
			parameters("char")), validLocation()
		);

		TS_ASSERT_EQUALS(storage.graph().getNodeCount(), 3);
		TS_ASSERT_EQUALS(storage.graph().getEdgeCount(), 2);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 4);
		TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 3);

		std::set<FilePath> files;
		files.insert(FilePath(m_filePath));
		storage.clearFileData(files);

		TS_ASSERT_EQUALS(storage.graph().getNodeCount(), 0);
		TS_ASSERT_EQUALS(storage.graph().getEdgeCount(), 0);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 0);
		TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 0);
	}

	void test_storage_clears_unreferenced_single_file_data_of_multi_file_storage()
	{
		m_filePath = "file.h";

		TestStorage storage;

		ParseFunction isTrue = ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("char"));
		storage.onFunctionParsed(validLocation(), isTrue, validLocation());

		m_filePath = "file.cpp";

		ParseFunction main = ParseFunction(typeUsage("int"), utility::splitToVector("main", "::"), parameters("void"));
		storage.onFunctionParsed(validLocation(), main, validLocation());

		storage.onCallParsed(validLocation(), main, isTrue);

		TS_ASSERT_EQUALS(storage.graph().getNodeCount(), 6);
		TS_ASSERT_EQUALS(storage.graph().getEdgeCount(), 5);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 9);
		TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 6);

		std::set<FilePath> files;
		files.insert(FilePath("file.cpp"));
		storage.clearFileData(files);

		TS_ASSERT_EQUALS(storage.graph().getNodeCount(), 3);
		TS_ASSERT_EQUALS(storage.graph().getEdgeCount(), 2);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 4);
		TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 3);
	}

	void test_storage_clears_referenced_single_file_data_of_multi_file_storage()
	{
		m_filePath = "file.h";

		TestStorage storage;

		ParseFunction isTrue = ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("void"));
		storage.onFunctionParsed(validLocation(), isTrue, validLocation());

		m_filePath = "file.cpp";

		ParseFunction main = ParseFunction(typeUsage("int"), utility::splitToVector("main", "::"), parameters("void"));
		storage.onFunctionParsed(validLocation(), main, validLocation());

		storage.onCallParsed(validLocation(), main, isTrue);

		TS_ASSERT_EQUALS(storage.graph().getNodeCount(), 5);
		TS_ASSERT_EQUALS(storage.graph().getEdgeCount(), 5);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 9);
		TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 5);

		std::set<FilePath> files;
		files.insert(FilePath("file.h"));
		storage.clearFileData(files);

		TS_ASSERT_EQUALS(storage.graph().getNodeCount(), 4);
		TS_ASSERT_EQUALS(storage.graph().getEdgeCount(), 3);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 5);
		TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 4);
	}

	void test_storage_clears_multi_file_data_of_multi_file_storage()
	{
		m_filePath = "file.h";

		TestStorage storage;

		ParseFunction isTrue = ParseFunction(typeUsage("bool"), utility::splitToVector("isTrue", "::"), parameters("void"));
		storage.onFunctionParsed(validLocation(), isTrue, validLocation());

		m_filePath = "file.cpp";

		ParseFunction main = ParseFunction(typeUsage("int"), utility::splitToVector("main", "::"), parameters("void"));
		storage.onFunctionParsed(validLocation(), main, validLocation());

		storage.onCallParsed(validLocation(), main, isTrue);

		TS_ASSERT_EQUALS(storage.graph().getNodeCount(), 5);
		TS_ASSERT_EQUALS(storage.graph().getEdgeCount(), 5);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 9);
		TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 5);

		std::set<FilePath> filePaths;
		filePaths.insert(FilePath("file.cpp"));
		filePaths.insert(FilePath("file.h"));
		storage.clearFileData(filePaths);

		TS_ASSERT_EQUALS(storage.graph().getNodeCount(), 0);
		TS_ASSERT_EQUALS(storage.graph().getEdgeCount(), 0);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 0);
		TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 0);
	}

	void test_storage_saves_file_nodes()
	{
		TestStorage storage;

		Id id = storage.onFileParsed("file.h");
		Node* node = storage.getNodeWithId(id);

		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getName(), "file.h");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_FILE);
	}

	void test_storage_saves_include_edge()
	{
		TestStorage storage;

		storage.onFileParsed("file.h");
		storage.onFileParsed("file.cpp");
		Id id = storage.onFileIncludeParsed(validLocation(7), "file.cpp", "file.h");

		Edge* edge = storage.getEdgeWithId(id);
		TS_ASSERT(edge);
		TS_ASSERT_EQUALS(edge->getType(), Edge::EDGE_INCLUDE);

		TS_ASSERT_EQUALS(edge->getFrom()->getName(), "file.cpp");
		TS_ASSERT_EQUALS(edge->getTo()->getName(), "file.h");

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 7));
	}

	void test_storage_finds_and_removes_depending_file_nodes()
	{
		TestStorage storage;

		Id id1 = storage.onFileParsed("f.h");
		Id id2 = storage.onFileParsed("file.h");
		Id id3 = storage.onFileParsed("file.cpp");
		Id id4 = storage.onFileIncludeParsed(validLocation(), "file.h", "f.h");
		Id id5 = storage.onFileIncludeParsed(validLocation(), "file.cpp", "file.h");

		std::string name1 = storage.getNodeWithId(id2)->getFullName();
		std::string name2 = storage.getNodeWithId(id3)->getFullName();

		std::set<FilePath> filePaths;
		filePaths.insert(FilePath(name1));
		std::set<FilePath> dependingFilePaths = storage.getDependingFilePathsAndRemoveFileNodes(filePaths);

		TS_ASSERT_EQUALS(dependingFilePaths.size(), 1);
		TS_ASSERT_EQUALS(dependingFilePaths.begin()->str(), name2);

		TS_ASSERT(storage.getNodeWithId(id1));
		TS_ASSERT(!storage.getNodeWithId(id2));
		TS_ASSERT(!storage.getNodeWithId(id3));
		TS_ASSERT(!storage.getEdgeWithId(id4));
		TS_ASSERT(!storage.getEdgeWithId(id5));
	}

private:
	class TestStorage
		: public Storage
	{
	public:
		Node* getNodeWithId(Id id) const
		{
			return dynamic_cast<Node*>(getGraph().getTokenById(id));
		}

		Edge* getEdgeWithId(Id id) const
		{
			return dynamic_cast<Edge*>(getGraph().getTokenById(id));
		}

		std::vector<TokenLocation*> getLocationsForId(Id id) const
		{
			const std::vector<Id>& locationIds = getGraph().getTokenById(id)->getLocationIds();

			std::vector<TokenLocation*> result;
			for (Id locationId : locationIds)
			{
				result.push_back(getTokenLocationCollection().findTokenLocationById(locationId));
			}

			return result;
		}

		const std::string& getWord(Id wordId) const
		{
			return getSearchIndex().getWord(wordId);
		}

		const Graph& graph() const
		{
			return getGraph();
		}

		const TokenLocationCollection& tokenLocationCollection() const
		{
			return getTokenLocationCollection();
		}

		const SearchIndex& searchIndex() const
		{
			return getSearchIndex();
		}
	};

	ParseLocation validLocation(Id locationId = 0) const
	{
		return ParseLocation(m_filePath, 1, locationId, 1, locationId);
	}

	bool isValidLocation(TokenLocation* location, Id locationId) const
	{
		return
			location->getFilePath() == m_filePath &&
			location->getLineNumber() == 1 &&
			location->getColumnNumber() == locationId;
	}

	ParseTypeUsage typeUsage(const std::string& typeName) const
	{
		return ParseTypeUsage(validLocation(), DataType(utility::splitToVector(typeName, "::")));
	}

	std::vector<ParseTypeUsage> parameters(const std::string& param) const
	{
		std::vector<ParseTypeUsage> params;
		params.push_back(typeUsage(param));
		return params;
	}

	std::string m_filePath;
};
