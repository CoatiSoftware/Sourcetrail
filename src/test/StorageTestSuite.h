#include "cxxtest/TestSuite.h"

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
	void test_storage_saves_typedef()
	{
		TestStorage storage;
		Id id = storage.onTypedefParsed(validLocation(1), "type", typeUsage("int"), ParserClient::ACCESS_NONE);

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
		Id id = storage.onClassParsed(validLocation(1), "Class", ParserClient::ACCESS_NONE, validLocation(2));

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
		Id id = storage.onStructParsed(validLocation(1), "Struct", ParserClient::ACCESS_NONE, validLocation(2));

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
		Id id = storage.onGlobalVariableParsed(validLocation(42), ParseVariable(typeUsage("char"), "Global", false));

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
		Id id = storage.onGlobalVariableParsed(validLocation(7), ParseVariable(typeUsage("char"), "Global", true));

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
			validLocation(3), ParseVariable(typeUsage("bool"), "m_field", false), ParserClient::ACCESS_NONE
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
			validLocation(11), ParseVariable(typeUsage("bool"), "Struct::m_field", false), ParserClient::ACCESS_PUBLIC
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
			validLocation(14), ParseFunction(typeUsage("bool"), "isTrue", parameters("char")), validLocation(41)
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
		TS_ASSERT_EQUALS(node->getComponent<TokenComponentSignature>()->getSignature(), "isTrue(char)");

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
			ParseFunction(typeUsage("void"), "isMethod", parameters("bool")),
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
		TS_ASSERT_EQUALS(node->getComponent<TokenComponentSignature>()->getSignature(), "isMethod(bool)");

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
			ParseFunction(typeUsage("void"), "isMethod", parameters("bool"), true),
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
			ParseFunction(typeUsage("void"), "Class::isMethod", parameters("bool")),
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
		Id id = storage.onNamespaceParsed(validLocation(1), "utility", validLocation(2));

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
		Id id = storage.onEnumParsed(validLocation(17), "Category", ParserClient::ACCESS_NONE, validLocation(23));

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
			storage.onEnumParsed(validLocation(1), "Class::Category", ParserClient::ACCESS_PRIVATE, validLocation(2));

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

	void test_storage_saves_enum_field()
	{
		TestStorage storage;
		Id id = storage.onEnumFieldParsed(validLocation(1), "VALUE");

		Node* node = storage.getNodeWithId(id);
		TS_ASSERT(node);
		TS_ASSERT_EQUALS(node->getFullName(), "VALUE");
		TS_ASSERT_EQUALS(node->getType(), Node::NODE_FIELD);

		std::vector<TokenLocation*> locations = storage.getLocationsForId(id);
		TS_ASSERT_EQUALS(locations.size(), 1);
		TS_ASSERT(isValidLocation(locations[0], 1));
	}

	void test_storage_saves_inheritance()
	{
		TestStorage storage;
		storage.onClassParsed(validLocation(), "ClassA", ParserClient::ACCESS_NONE, validLocation());
		storage.onClassParsed(validLocation(), "ClassB", ParserClient::ACCESS_NONE, validLocation());
		Id id = storage.onInheritanceParsed(validLocation(5), "ClassB", "ClassA", ParserClient::ACCESS_PUBLIC);

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

	void test_storage_saves_call()
	{
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), "isTrue", parameters("char")), validLocation()
		);
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("void"), "func", parameters("bool")), validLocation()
		);
		Id id = storage.onCallParsed(
			validLocation(9),
			ParseFunction(typeUsage("bool"), "isTrue", parameters("char")),
			ParseFunction(typeUsage("void"), "func", parameters("bool"))
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
		storage.onGlobalVariableParsed(validLocation(), ParseVariable(typeUsage("bool"), "global", false));
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), "isTrue", parameters("char")), validLocation()
		);

		Id id = storage.onCallParsed(
			validLocation(7),
			ParseVariable(typeUsage("bool"), "global", false),
			ParseFunction(typeUsage("bool"), "isTrue", parameters("char"))
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
			validLocation(), ParseFunction(typeUsage("bool"), "isTrue", parameters("char")), validLocation()
		);
		storage.onFieldParsed(
			validLocation(), ParseVariable(typeUsage("bool"), "Foo::m_field", false), ParserClient::ACCESS_PRIVATE
		);

		Id id = storage.onFieldUsageParsed(
			validLocation(7),
			ParseFunction(typeUsage("bool"), "isTrue", parameters("char")),
			"Foo::m_field"
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
			validLocation(), ParseFunction(typeUsage("bool"), "isTrue", parameters("char")), validLocation()
		);
		storage.onGlobalVariableParsed(validLocation(), ParseVariable(typeUsage("bool"), "global", false));

		Id id = storage.onGlobalVariableUsageParsed(
			validLocation(7),
			ParseFunction(typeUsage("bool"), "isTrue", parameters("char")),
			"global"
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
			validLocation(), ParseFunction(typeUsage("bool"), "isTrue", parameters("char")), validLocation()
		);
		storage.onStructParsed(validLocation(), "Struct", ParserClient::ACCESS_NONE, validLocation());

		Id id = storage.onTypeUsageParsed(
			typeUsage("Struct"),
			ParseFunction(typeUsage("bool"), "isTrue", parameters("char"))
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

private:
	class TestStorage
		: public Storage
	{
	public:
		Node* getNodeWithId(Id id) const
		{
			return dynamic_cast<Node*>(getTokenWithId(id));
		}

		Edge* getEdgeWithId(Id id) const
		{
			return dynamic_cast<Edge*>(getTokenWithId(id));
		}

		std::vector<TokenLocation*> getLocationsForId(Id id) const
		{
			return getTokenLocationsForId(id);
		}
	};

	ParseLocation validLocation(Id locationId = 0) const
	{
		return ParseLocation("file.cpp", 1, locationId, 1, locationId);
	}

	bool isValidLocation(TokenLocation* location, Id locationId) const
	{
		return
			location->getFilePath() == "file.cpp" &&
			location->getLineNumber() == 1 &&
			location->getColumnNumber() == locationId;
	}

	ParseTypeUsage typeUsage(const std::string& typeName) const
	{
		return ParseTypeUsage(validLocation(), DataType(typeName));
	}

	std::vector<ParseTypeUsage> parameters(const std::string& param) const
	{
		std::vector<ParseTypeUsage> params;
		params.push_back(typeUsage(param));
		return params;
	}
};
