#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"

#include "data/graph/token_component/TokenComponentAbstraction.h"
#include "data/graph/token_component/TokenComponentAccess.h"
#include "data/graph/token_component/TokenComponentStatic.h"
#include "data/location/TokenLocation.h"
#include "data/parser/ParseLocation.h"
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
		Id id = storage.onTypedefParsed(validLocation(1), createNameHierarchy("type"), ParserClient::ACCESS_NONE);

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "type");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_TYPEDEF);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_class()
	{
		TestStorage storage;
		Id id = storage.onClassParsed(validLocation(1), createNameHierarchy("Class"), ParserClient::ACCESS_NONE, validLocation(2));

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "Class");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_CLASS);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
		TS_ASSERT_EQUALS(tlc->getTokenLocations().find(2)->second->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_struct()
	{
		TestStorage storage;
		Id id = storage.onStructParsed(validLocation(1), createNameHierarchy("Struct"), ParserClient::ACCESS_NONE, validLocation(2));

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "Struct");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_STRUCT);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
		TS_ASSERT_EQUALS(tlc->getTokenLocations().find(2)->second->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_global_variable()
	{
		TestStorage storage;
		Id id = storage.onGlobalVariableParsed(validLocation(42), createNameHierarchy("Global"));

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "Global");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_GLOBAL_VARIABLE);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_field()
	{
		TestStorage storage;
		Id id = storage.onFieldParsed(
			validLocation(3), createNameHierarchy("m_field"), ParserClient::ACCESS_NONE);
		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "m_field");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_FIELD);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_field_as_member()
	{
		NameHierarchy a = createNameHierarchy("Struct::m_field");
		NameHierarchy b = createNameHierarchy("Struct");

		TestStorage storage;
		Id id = storage.onFieldParsed(
			validLocation(11), a, ParserClient::ACCESS_PUBLIC
		);

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "Struct::m_field");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_FIELD);

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_MEMBER, b, a) != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_function()
	{
		TestStorage storage;
		Id id = storage.onFunctionParsed(
			validLocation(14), createFunctionNameHierarchy("bool", "isTrue", "(char)"), validLocation(41)
		);

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "bool isTrue(char)");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_FUNCTION);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
	}

	void test_storage_saves_method()
	{
		TestStorage storage;
		Id id = storage.onMethodParsed(
			validLocation(9),
			createFunctionNameHierarchy("void", "isMethod", "(bool)"),
			ParserClient::ACCESS_NONE,
			ParserClient::ABSTRACTION_NONE,
			validLocation(4)
		);

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "void isMethod(bool)");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_METHOD);

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
		//TS_ASSERT_EQUALS(node->getQualifiedNameWithSignature(), "isMethod");
		//TS_ASSERT_EQUALS(node->getType(), Node::NODE_METHOD);
		//TS_ASSERT(node->getComponent<TokenComponentStatic>());
	}

	void test_storage_saves_method_as_member()
	{
		NameHierarchy a = createFunctionNameHierarchy("void", "Class::isMethod", "(bool)");
		TestStorage storage;
		storage.onMethodParsed(
			validLocation(1),
			a,
			ParserClient::ACCESS_PROTECTED,
			ParserClient::ABSTRACTION_VIRTUAL,
			validLocation(4)
		);

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_MEMBER, createNameHierarchy("Class"), a) != 0);
	}

	void test_storage_saves_namespace()
	{
		TestStorage storage;
		Id id = storage.onNamespaceParsed(validLocation(1), createNameHierarchy("utility"), validLocation(2));

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "utility");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_NAMESPACE);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
		TS_ASSERT_EQUALS(tlc->getTokenLocations().find(2)->second->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_enum()
	{
		TestStorage storage;
		Id id = storage.onEnumParsed(validLocation(17), createNameHierarchy("Category"), ParserClient::ACCESS_NONE, validLocation(23));

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "Category");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_ENUM);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 2);
		TS_ASSERT_EQUALS(tlc->getTokenLocations().find(2)->second->getType(), TokenLocation::LOCATION_SCOPE);
	}

	void test_storage_saves_enum_as_member()
	{
		NameHierarchy a = createNameHierarchy("Class::Category");
		TestStorage storage;
		storage.onEnumParsed(
			validLocation(1), a,
			ParserClient::ACCESS_PRIVATE, validLocation(2)
		);

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_MEMBER, createNameHierarchy("Class"), a) != 0);
	}

	void test_storage_saves_enum_constant()
	{
		TestStorage storage;
		Id id = storage.onEnumConstantParsed(validLocation(1), createNameHierarchy("VALUE"));

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "VALUE");
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

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_INHERITANCE, createNameHierarchy("ClassB"), createNameHierarchy("ClassA")) != 0);
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

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_INHERITANCE, createNameHierarchy("StructB"), createNameHierarchy("StructA")) != 0);

		//TS_ASSERT(edge->getComponent<TokenComponentAccess>());
		//TS_ASSERT_EQUALS(edge->getComponent<TokenComponentAccess>()->getAccess(), TokenComponentAccess::ACCESS_PUBLIC);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_method_override()
	{
		TestStorage storage;

		NameHierarchy a = createFunctionNameHierarchy("void", "A::isMethod", "(bool)");
		NameHierarchy b = createFunctionNameHierarchy("void", "B::isMethod", "(bool)");

		storage.onMethodParsed(
			validLocation(9),
			a,
			ParserClient::ACCESS_PRIVATE,
			ParserClient::ABSTRACTION_VIRTUAL,
			validLocation(4));
		storage.onMethodParsed(
			validLocation(7),
			b,
			ParserClient::ACCESS_PRIVATE,
			ParserClient::ABSTRACTION_NONE,
			validLocation(3));

		storage.onMethodOverrideParsed(validLocation(4), a, b);

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_OVERRIDE, b, a) != 0);
	}

	void test_storage_saves_call()
	{
		TestStorage storage;

		NameHierarchy a = createFunctionNameHierarchy("bool", "isTrue", "(char)");
		NameHierarchy b = createFunctionNameHierarchy("void", "func", "(bool)");

		storage.onFunctionParsed(
			validLocation(), a, validLocation()
		);
		storage.onFunctionParsed(
			validLocation(), b, validLocation()
		);
		Id id = storage.onCallParsed(
			validLocation(9),
			a,
			b
		);

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_CALL, a, b) != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_call_in_global_variable_declaration()
	{

		TestStorage storage;
		storage.onGlobalVariableParsed(validLocation(), createNameHierarchy("global"));

		NameHierarchy a = createFunctionNameHierarchy("bool", "isTrue", "(char)");
		storage.onFunctionParsed(
			validLocation(), a, validLocation()
		);

		Id id = storage.onCallParsed(
			validLocation(7),
			createNameHierarchy("global"),
			a
		);

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_CALL, createNameHierarchy("global"), a) != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_field_usage()
	{
		NameHierarchy a = createFunctionNameHierarchy("bool", "isTrue", "(char)");

		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), a, validLocation()
		);
		storage.onFieldParsed(
			validLocation(), createNameHierarchy("Foo::m_field"), ParserClient::ACCESS_PRIVATE
		);

		Id id = storage.onFieldUsageParsed(
			validLocation(7),
			a,
			createNameHierarchy("Foo::m_field")
		);

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_USAGE, a, createNameHierarchy("Foo::m_field")) != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_global_variable_usage()
	{
		NameHierarchy a = createFunctionNameHierarchy("bool", "isTrue", "(char)");

		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), a, validLocation()
		);
		storage.onGlobalVariableParsed(validLocation(), createNameHierarchy("global"));

		Id id = storage.onGlobalVariableUsageParsed(
			validLocation(7),
			a,
			createNameHierarchy("global")
		);

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_USAGE, a, createNameHierarchy("global")) != 0);

		std::shared_ptr<TokenLocationCollection> tlc = storage.getLocationCollectionForTokenId(id);
		TS_ASSERT_EQUALS(tlc->getTokenLocationCount(), 1);
	}

	void test_storage_saves_type_usage()
	{
		NameHierarchy a = createFunctionNameHierarchy("Struct", "isTrue", "(char)");
		TestStorage storage;
		storage.onFunctionParsed(
			validLocation(), a, validLocation()
		);
		storage.onStructParsed(
			validLocation(), createNameHierarchy("Struct"), ParserClient::ACCESS_NONE, validLocation());

		Id id = storage.onTypeUsageParsed(
			validLocation(7), a, createNameHierarchy("Struct")
		);

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_TYPE_USAGE, a, createNameHierarchy("Struct")) != 0);

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

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeWithId(id).getQualifiedNameWithSignature(), "file.h");
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_FILE);
	}

	void test_storage_saves_include_edge()
	{
		TestStorage storage;

		storage.onFileParsed(FileInfo("file.h"));
		storage.onFileParsed(FileInfo("file.cpp"));
		Id id = storage.onFileIncludeParsed(validLocation(7), FileInfo("file.cpp"), FileInfo("file.h"));

		TS_ASSERT(storage.getEdgeId(Edge::EDGE_INCLUDE, createNameHierarchy("file.cpp"), createNameHierarchy("file.h")) != 0);

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

		//std::string name1 = storage.getNodeWithId(id2)->getQualifiedNameWithSignature();
		//std::string name2 = storage.getNodeWithId(id3)->getQualifiedNameWithSignature();

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

		Id getEdgeId(Edge::EdgeType type, const NameHierarchy& fromName, const NameHierarchy& toName) const
		{
			NameHierarchy from;

			return getIdForEdge(type, fromName, toName);
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

	NameHierarchy createFunctionNameHierarchy(std::string ret, std::string name, std::string parameters) const
	{
		NameHierarchy nameHierarchy = createNameHierarchy(name);
		std::string lastName = nameHierarchy.back()->getName();
		nameHierarchy.pop();
		nameHierarchy.push(std::make_shared<NameElement>(lastName, NameElement::Signature(ret, parameters)));
		return nameHierarchy;
	}

	NameHierarchy createNameHierarchy(std::string s) const
	{
		NameHierarchy nameHierarchy;
		for (std::string element: utility::splitToVector(s, "::"))
		{
			nameHierarchy.push(std::make_shared<NameElement>(element, NameElement::Signature()));
		}
		return nameHierarchy;
	}

	std::string m_filePath;
};
