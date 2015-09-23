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
#include "data/type/NamedDataType.h"

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
		Id id = storage.onTypedefParsed(validLocation(1), createNameHierarchy("type"), typeUsage("int"), ParserClient::ACCESS_NONE);

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "type");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_TYPEDEF);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_TYPEDEF_OF) + ":type->int") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_class()
	{
		TestStorage storage;
		Id id = storage.onClassParsed(validLocation(1), createNameHierarchy("Class"), ParserClient::ACCESS_NONE, validLocation(2));

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "Class");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_CLASS);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
		TS_ASSERT_EQUALS(tlc->getTokenLocations().find(2)->second->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_struct()
	{
		TestStorage storage;
		Id id = storage.onStructParsed(validLocation(1), createNameHierarchy("Struct"), ParserClient::ACCESS_NONE, validLocation(2));

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "Struct");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_STRUCT);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
		TS_ASSERT_EQUALS(tlc->getTokenLocations().find(2)->second->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_global_variable()
	{
		TestStorage storage;
		Id id = storage.onGlobalVariableParsed(validLocation(42), ParseVariable(typeUsage("char"), createNameHierarchy("Global"), false));

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "Global");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_GLOBAL_VARIABLE);

		//TS_ASSERT(!node->getComponent<TokenComponentStatic>());

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_TYPE_OF) + ":Global->char") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_global_variable_static()
	{
		TestStorage storage;
		Id id = storage.onGlobalVariableParsed(validLocation(7), ParseVariable(typeUsage("char"), createNameHierarchy("Global"), true));

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "Global");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_GLOBAL_VARIABLE);

		//TS_ASSERT(node->getComponent<TokenComponentStatic>());

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_field()
	{
		TestStorage storage;
		Id id = storage.onFieldParsed(
			validLocation(3), ParseVariable(typeUsage("bool"), createNameHierarchy("m_field"), false), ParserClient::ACCESS_NONE
		);

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "m_field");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_FIELD);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_TYPE_OF) + ":m_field->bool") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_field_as_member()
	{
		TestStorage storage;
		Id id = storage.onFieldParsed(
			validLocation(11), ParseVariable(typeUsage("bool"), createNameHierarchy("Struct::m_field"), false), ParserClient::ACCESS_PUBLIC
		);

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "Struct::m_field");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_FIELD);


		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_MEMBER) + ":Struct->Struct::m_field") != 0);
		//TS_ASSERT(memberEdge->getComponent<TokenComponentAccess>());
		//TS_ASSERT_EQUALS(
		//	memberEdge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PUBLIC
		//);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_TYPE_OF) + ":Struct::m_field->bool") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_function()
	{
		TestStorage storage;
		Id id = storage.onFunctionParsed(
			validLocation(14), ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char")), validLocation(41)
		);

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "isTrue");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_FUNCTION);
		//TS_ASSERT(node->getComponent<TokenComponentSignature>());
		//TS_ASSERT_EQUALS(storage.getWord(node->getComponent<TokenComponentSignature>()->getWordId()), "isTrue(char)");

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_RETURN_TYPE_OF) + ":isTrue->bool") != 0);
		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_PARAMETER_TYPE_OF) + ":isTrue->char") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
		TS_ASSERT_EQUALS(tlc->getTokenLocations().find(4)->second->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_method()
	{
		TestStorage storage;
		Id id = storage.onMethodParsed(
			validLocation(9),
			ParseFunction(typeUsage("void"), createNameHierarchy("isMethod"), parameters("bool")),
			ParserClient::ACCESS_NONE,
			ParserClient::ABSTRACTION_NONE,
			validLocation(4)
		);

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "isMethod");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_METHOD);
		//TS_ASSERT(node->getComponent<TokenComponentSignature>());
		//TS_ASSERT_EQUALS(storage.getWord(node->getComponent<TokenComponentSignature>()->getWordId()), "isMethod(bool)");

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_RETURN_TYPE_OF) + ":isMethod->void") != 0);
		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_PARAMETER_TYPE_OF) + ":isMethod->bool") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
	}

	void test_storage_saves_method_static()
	{
		//TestStorage storage;
		//Id id = storage.onMethodParsed(
		//	validLocation(1),
		//	ParseFunction(typeUsage("void"), createNameHierarchy("isMethod"), parameters("bool"), true),
		//	ParserClient::ACCESS_NONE,
		//	ParserClient::ABSTRACTION_NONE,
		//	validLocation(4)
		//);

		//Node* node = storage.getNodeWithId(id);
		//TS_ASSERT(node);
		//TS_ASSERT_EQUALS(node->getFullName(), "isMethod");
		//TS_ASSERT_EQUALS(node->getType(), Node::NODE_METHOD);
		//TS_ASSERT(node->getComponent<TokenComponentStatic>());
	}

	void test_storage_saves_method_as_member()
	{
		TestStorage storage;
		storage.onMethodParsed(
			validLocation(1),
			ParseFunction(typeUsage("void"), createNameHierarchy("Class::isMethod"), parameters("bool")),
			ParserClient::ACCESS_PROTECTED,
			ParserClient::ABSTRACTION_VIRTUAL,
			validLocation(4)
		);

		//TS_ASSERT(node->getComponent<TokenComponentAbstraction>());
		//TS_ASSERT_EQUALS(
		//	node->getComponent<TokenComponentAbstraction>()->getAbstraction(),
		//	TokenComponentAbstraction::ABSTRACTION_VIRTUAL
		//);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_MEMBER) + ":Class->Class::isMethod") != 0);
		//TS_ASSERT(memberEdge->getComponent<TokenComponentAccess>());
		//TS_ASSERT_EQUALS(
		//	memberEdge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PROTECTED
		//);
	}

	void test_storage_saves_namespace()
	{
		TestStorage storage;
		Id id = storage.onNamespaceParsed(validLocation(1), createNameHierarchy("utility"), validLocation(2));

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "utility");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_NAMESPACE);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
		TS_ASSERT_EQUALS(tlc->getTokenLocations().find(2)->second->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_enum()
	{
		TestStorage storage;
		Id id = storage.onEnumParsed(validLocation(17), createNameHierarchy("Category"), ParserClient::ACCESS_NONE, validLocation(23));

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "Category");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_ENUM);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
		TS_ASSERT_EQUALS(tlc->getTokenLocations().find(2)->second->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_enum_as_member()
	{
		TestStorage storage;
		storage.onEnumParsed(
			validLocation(1), createNameHierarchy("Class::Category"),
			ParserClient::ACCESS_PRIVATE, validLocation(2)
		);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_MEMBER) + ":Class->Class::Category") != 0);

		//TS_ASSERT(memberEdge->getComponent<TokenComponentAccess>());
		//TS_ASSERT_EQUALS(
		//	memberEdge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PRIVATE
		//);
	}

	void test_storage_saves_enum_constant()
	{
		TestStorage storage;
		Id id = storage.onEnumConstantParsed(validLocation(1), createNameHierarchy("VALUE"));

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "VALUE");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_ENUM_CONSTANT);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_class_inheritance()
	{
		TestStorage storage;
		storage.onClassParsed(validLocation(), createNameHierarchy("ClassA"), ParserClient::ACCESS_NONE, validLocation());
		storage.onClassParsed(validLocation(), createNameHierarchy("ClassB"), ParserClient::ACCESS_NONE, validLocation());
		Id id =
			storage.onInheritanceParsed(validLocation(5), createNameHierarchy("ClassB"),
			createNameHierarchy("ClassA"), ParserClient::ACCESS_PUBLIC
		);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_INHERITANCE) + ":ClassB->ClassA") != 0);
		//TS_ASSERT(edge->getComponent<TokenComponentAccess>());
		//TS_ASSERT_EQUALS(edge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PUBLIC);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_struct_inheritance()
	{
		TestStorage storage;
		storage.onStructParsed(validLocation(), createNameHierarchy("StructA"), ParserClient::ACCESS_NONE, validLocation());
		storage.onStructParsed(validLocation(), createNameHierarchy("StructB"), ParserClient::ACCESS_NONE, validLocation());
		Id id =
			storage.onInheritanceParsed(validLocation(5), createNameHierarchy("StructB"),
			createNameHierarchy("StructA"), ParserClient::ACCESS_PUBLIC
		);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_INHERITANCE) + ":StructB->StructA") != 0);

		//TS_ASSERT(edge->getComponent<TokenComponentAccess>());
		//TS_ASSERT_EQUALS(edge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PUBLIC);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_method_override()
	{
		TestStorage storage;

		ParseFunction a(typeUsage("void"), createNameHierarchy("A::isMethod"), parameters("bool"));
		ParseFunction b(typeUsage("void"), createNameHierarchy("B::isMethod"), parameters("bool"));

		storage.onMethodParsed(validLocation(9), a, ParserClient::ACCESS_PRIVATE, ParserClient::ABSTRACTION_VIRTUAL, validLocation(4));
		storage.onMethodParsed(validLocation(7), b, ParserClient::ACCESS_PRIVATE, ParserClient::ABSTRACTION_NONE, validLocation(3));

		storage.onMethodOverrideParsed(validLocation(4), a, b);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_OVERRIDE) + ":B::isMethod->A::isMethod") != 0);
	}

	void test_storage_saves_call()
	{
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char")), validLocation()
		);
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("void"), createNameHierarchy("func"), parameters("bool")), validLocation()
		);
		Id id = storage.onCallParsed(
			validLocation(9),
			ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char")),
			ParseFunction(typeUsage("void"), createNameHierarchy("func"), parameters("bool"))
		);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_CALL) + ":isTrue->func") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_call_in_global_variable_declaration()
	{
		TestStorage storage;
		storage.onGlobalVariableParsed(validLocation(), ParseVariable(typeUsage("bool"), createNameHierarchy("global"), false));
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char")), validLocation()
		);

		Id id = storage.onCallParsed(
			validLocation(7),
			ParseVariable(typeUsage("bool"), createNameHierarchy("global"), false),
			ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char"))
		);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_CALL) + ":global->isTrue") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_field_usage()
	{
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char")), validLocation()
		);
		storage.onFieldParsed(
			validLocation(), ParseVariable(typeUsage("bool"), createNameHierarchy("Foo::m_field"), false), ParserClient::ACCESS_PRIVATE
		);

		Id id = storage.onFieldUsageParsed(
			validLocation(7),
			ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char")),
			createNameHierarchy("Foo::m_field")
		);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_USAGE) + ":isTrue->Foo::m_field") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_global_variable_usage()
	{
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char")), validLocation()
		);
		storage.onGlobalVariableParsed(validLocation(), ParseVariable(typeUsage("bool"), createNameHierarchy("global"), false));

		Id id = storage.onGlobalVariableUsageParsed(
			validLocation(7),
			ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char")),
			createNameHierarchy("global")
		);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_USAGE) + ":isTrue->global") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_type_usage()
	{
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"),
			parameters("char")), validLocation()
		);
		storage.onStructParsed(
			validLocation(), createNameHierarchy("Struct"), ParserClient::ACCESS_NONE, validLocation());

		Id id = storage.onTypeUsageParsed(
			typeUsage("Struct"),
			ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char"))
		);

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_TYPE_USAGE) + ":isTrue->Struct") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_clears_single_file_data_of_single_file_storage()
	{
		/*TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"),
			parameters("char")), validLocation()
		);

		TS_ASSERT_EQUALS(storage.getNodeCount(), 3);
		TS_ASSERT_EQUALS(storage.getEdgeCount(), 2);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 4);
		TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 3);

		std::set<FilePath> files;
		files.insert(FilePath(m_filePath));
		storage.clearFileData(files);

		TS_ASSERT_EQUALS(storage.getNodeCount(), 0);
		TS_ASSERT_EQUALS(storage.getEdgeCount(), 0);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 0);
		TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 0);*/
	}

	void test_storage_clears_unreferenced_single_file_data_of_multi_file_storage()
	{
		//m_filePath = "file.h";

		//TestStorage storage;

		//ParseFunction isTrue = ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("char"));
		//storage.onFunctionParsed(validLocation(), isTrue, validLocation());

		//m_filePath = "file.cpp";

		//ParseFunction main = ParseFunction(typeUsage("int"), createNameHierarchy("main"), parameters("void"));
		//storage.onFunctionParsed(validLocation(), main, validLocation());

		//storage.onCallParsed(validLocation(), main, isTrue);

		//TS_ASSERT_EQUALS(storage.getNodeCount(), 6);
		//TS_ASSERT_EQUALS(storage.getEdgeCount(), 5);
		//TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 9);
		//TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 6);

		//std::set<FilePath> files;
		//files.insert(FilePath("file.cpp"));
		//storage.clearFileData(files);

		//TS_ASSERT_EQUALS(storage.getNodeCount(), 3);
		//TS_ASSERT_EQUALS(storage.getEdgeCount(), 2);
		//TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 4);
		//TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 3);
	}

	void test_storage_clears_referenced_single_file_data_of_multi_file_storage()
	{
		//m_filePath = "file.h";

		//TestStorage storage;

		//ParseFunction isTrue = ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("void"));
		//storage.onFunctionParsed(validLocation(), isTrue, validLocation());

		//m_filePath = "file.cpp";

		//ParseFunction main = ParseFunction(typeUsage("int"), createNameHierarchy("main"), parameters("void"));
		//storage.onFunctionParsed(validLocation(), main, validLocation());

		//storage.onCallParsed(validLocation(), main, isTrue);

		//TS_ASSERT_EQUALS(storage.getNodeCount(), 5);
		//TS_ASSERT_EQUALS(storage.getEdgeCount(), 5);
		//TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 9);
		//TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 5);

		//std::set<FilePath> files;
		//files.insert(FilePath("file.h"));
		//storage.clearFileData(files);

		//TS_ASSERT_EQUALS(storage.getNodeCount(), 4);
		//TS_ASSERT_EQUALS(storage.getEdgeCount(), 3);
		//TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 5);
		//TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 4);
	}

	void test_storage_clears_multi_file_data_of_multi_file_storage()
	{
		//m_filePath = "file.h";

		//TestStorage storage;

		//ParseFunction isTrue = ParseFunction(typeUsage("bool"), createNameHierarchy("isTrue"), parameters("void"));
		//storage.onFunctionParsed(validLocation(), isTrue, validLocation());

		//m_filePath = "file.cpp";

		//ParseFunction main = ParseFunction(typeUsage("int"), createNameHierarchy("main"), parameters("void"));
		//storage.onFunctionParsed(validLocation(), main, validLocation());

		//storage.onCallParsed(validLocation(), main, isTrue);

		//TS_ASSERT_EQUALS(storage.getNodeCount(), 5);
		//TS_ASSERT_EQUALS(storage.getEdgeCount(), 5);
		//TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 9);
		//TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 5);

		//std::set<FilePath> filePaths;
		//filePaths.insert(FilePath("file.cpp"));
		//filePaths.insert(FilePath("file.h"));
		//storage.clearFileData(filePaths);

		//TS_ASSERT_EQUALS(storage.getNodeCount(), 0);
		//TS_ASSERT_EQUALS(storage.getEdgeCount(), 0);
		//TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 0);
		//TS_ASSERT_EQUALS(storage.searchIndex().getNodeCount(), 0);
	}

	void test_storage_saves_file_nodes()
	{
		TestStorage storage;

		Id id = storage.onFileParsed(FileInfo("file.h"));

		TS_ASSERT_EQUALS(storage.getNameForNodeWithId(id), "file.h");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_FILE);
	}

	void test_storage_saves_include_edge()
	{
		TestStorage storage;

		storage.onFileParsed(FileInfo("file.h"));
		storage.onFileParsed(FileInfo("file.cpp"));
		Id id = storage.onFileIncludeParsed(validLocation(7), FileInfo("file.cpp"), FileInfo("file.h"));

		TS_ASSERT(storage.getIdForEdgeWithName(Edge::getTypeString(Edge::EDGE_INCLUDE) + ":file.cpp->file.h") != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_finds_and_removes_depending_file_nodes()
	{
		//TestStorage storage;

		//Id id1 = storage.onFileParsed("f.h");
		//Id id2 = storage.onFileParsed("file.h");
		//Id id3 = storage.onFileParsed("file.cpp");
		//Id id4 = storage.onFileIncludeParsed(validLocation(), "file.h", "f.h");
		//Id id5 = storage.onFileIncludeParsed(validLocation(), "file.cpp", "file.h");

		//std::string name1 = storage.getNodeWithId(id2)->getFullName();
		//std::string name2 = storage.getNodeWithId(id3)->getFullName();

		//std::set<FilePath> filePaths;
		//filePaths.insert(FilePath(name1));
		//std::set<FilePath> dependingFilePaths = storage.getDependingFilePathsAndRemoveFileNodes(filePaths);

		//TS_ASSERT_EQUALS(dependingFilePaths.size(), 1);
		//TS_ASSERT_EQUALS(dependingFilePaths.begin()->str(), name2);

		//TS_ASSERT(storage.getNodeWithId(id1));
		//TS_ASSERT(!storage.getNodeWithId(id2));
		//TS_ASSERT(!storage.getNodeWithId(id3));
		//TS_ASSERT(!storage.getEdgeWithId(id4));
		//TS_ASSERT(!storage.getEdgeWithId(id5));
	}

private:
	class TestStorage
		: public Storage
	{
	public:
		TestStorage()
			: Storage("data/test.sqlite")
		{
			clear();
			onFileParsed(FileInfo("file.cpp"));
		}

		std::shared_ptr<TokenLocationCollection> getLocationCollectionForTokenId(Id id) const
		{
			std::vector<Id> tokenIds;
			tokenIds.push_back(id);
			return getTokenLocationsForTokenIds(tokenIds);
		}

		//const std::string& getWord(Id wordId) const
		//{
		//	return getSearchIndex().getWord(wordId);
		//}

		//const size_t getNodeCount() const
		//{
		//	return getGraph().getNodeCount();
		//}

		//const size_t getEdgeCount() const
		//{
		//	return getGraph().getEdgeCount();
		//}

		const SearchIndex& searchIndex() const
		{
			return getSearchIndex();
		}
	};

	ParseLocation validLocation(Id locationId = 0) const // id is not used as id ..... who programs this? ebsi? :P
	{
		return ParseLocation(m_filePath, 1, locationId, 1, locationId);
	}

	bool isValidLocation(TokenLocation* location, Id locationId) const // remove this one
	{
		return
			location->getFilePath() == m_filePath &&
			location->getLineNumber() == 1 &&
			location->getColumnNumber() == locationId;
	}

	NameHierarchy createNameHierarchy(std::string s) const
	{
		NameHierarchy nameHierarchy;
		for (std::string element: utility::splitToVector(s, "::"))
		{
			nameHierarchy.push(std::make_shared<NameElement>(element));
		}
		return nameHierarchy;
	}

	ParseTypeUsage typeUsage(const std::string& typeName) const
	{
		return ParseTypeUsage(validLocation(), std::make_shared<NamedDataType>(createNameHierarchy(typeName)));
	}

	std::vector<ParseTypeUsage> parameters(const std::string& param) const
	{
		std::vector<ParseTypeUsage> params;
		params.push_back(typeUsage(param));
		return params;
	}

	std::string m_filePath;
};
