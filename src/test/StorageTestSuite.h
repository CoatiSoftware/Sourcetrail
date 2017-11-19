#include "cxxtest/TestSuite.h"

#include "utility/utilityString.h"

#include "data/parser/ParseLocation.h"
#include "data/storage/IntermediateStorage.h"
#include "data/storage/PersistentStorage.h"

class StorageTestSuite: public CxxTest::TestSuite
{
public:
	void setUp()
	{
		m_filePath = FilePath("file.cpp");
	}

	void test_storage_saves_file()
	{
		TestStorage storage;

		std::string filePath = "path/to/test.h";

		std::shared_ptr<IntermediateStorage> intermetiateStorage = std::make_shared<IntermediateStorage>();
		Id id = intermetiateStorage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_FILE), NameHierarchy::serialize(NameHierarchy(filePath, NAME_DELIMITER_FILE))));
		intermetiateStorage->addFile(StorageFile(id, filePath, "someTime", true));

		storage.inject(intermetiateStorage.get());

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeId(id).getQualifiedName(), filePath);
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(id), Node::NODE_FILE);

	}
	void test_storage_saves_node()
	{
		NameHierarchy a = createNameHierarchy("type");

		TestStorage storage;

		std::shared_ptr<IntermediateStorage> intermetiateStorage = std::make_shared<IntermediateStorage>();
		intermetiateStorage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_TYPEDEF), NameHierarchy::serialize(a)));

		storage.inject(intermetiateStorage.get());

		Id storedId = storage.getNodeIdForNameHierarchy(a);

		TS_ASSERT(storedId != 0);
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(storedId), Node::NODE_TYPEDEF);
	}


	void test_storage_saves_field_as_member()
	{
		NameHierarchy a = createNameHierarchy("Struct");
		NameHierarchy b = createNameHierarchy("Struct::m_field");

		TestStorage storage;

		std::shared_ptr<IntermediateStorage> intermetiateStorage = std::make_shared<IntermediateStorage>();

		Id aId = intermetiateStorage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_STRUCT), NameHierarchy::serialize(a)));
		intermetiateStorage->addSymbol(StorageSymbol(aId, DEFINITION_EXPLICIT));

		Id bId = intermetiateStorage->addNode(StorageNodeData(Node::typeToInt(Node::NODE_FIELD), NameHierarchy::serialize(b)));
		intermetiateStorage->addSymbol(StorageSymbol(bId, DEFINITION_EXPLICIT));
		intermetiateStorage->addEdge(StorageEdgeData(Edge::typeToInt(Edge::EDGE_MEMBER), aId, bId));

		storage.inject(intermetiateStorage.get());

		TS_ASSERT(storage.getIdForEdge(Edge::EDGE_MEMBER, a, b) != 0);
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

		std::set<FilePath> files;
		files.insert(FilePath(m_filePath));
		storage.clearFileData(files);

		TS_ASSERT_EQUALS(storage.getNodeCount(), 0);
		TS_ASSERT_EQUALS(storage.getEdgeCount(), 0);
		TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 0);*/
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

		//std::set<FilePath> files;
		//files.insert(FilePath("file.cpp"));
		//storage.clearFileData(files);

		//TS_ASSERT_EQUALS(storage.getNodeCount(), 3);
		//TS_ASSERT_EQUALS(storage.getEdgeCount(), 2);
		//TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 4);*/
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

		//std::set<FilePath> files;
		//files.insert(FilePath("file.h"));
		//storage.clearFileData(files);

		//TS_ASSERT_EQUALS(storage.getNodeCount(), 4);
		//TS_ASSERT_EQUALS(storage.getEdgeCount(), 3);
		//TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 5);
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

		//std::set<FilePath> filePaths;
		//filePaths.insert(FilePath("file.cpp"));
		//filePaths.insert(FilePath("file.h"));
		//storage.clearFileData(filePaths);

		//TS_ASSERT_EQUALS(storage.getNodeCount(), 0);
		//TS_ASSERT_EQUALS(storage.getEdgeCount(), 0);
		//TS_ASSERT_EQUALS(storage.tokenLocationCollection().getTokenLocations().size(), 0);
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
		: public PersistentStorage
	{
	public:
		TestStorage()
			: PersistentStorage(FilePath("data/test.sqlite"), FilePath("data/testBookmarks.sqlite"))
		{
			clear();
		}

		//const size_t getNodeCount() const
		//{
		//	return getGraph().getNodeCount();
		//}

		//const size_t getEdgeCount() const
		//{
		//	return getGraph().getEdgeCount();
		//}

		Id getEdgeId(Edge::EdgeType type, const NameHierarchy& fromName, const NameHierarchy& toName) const
		{
			return getIdForEdge(type, fromName, toName);
		}
	};

	ParseLocation validLocation(Id locationId = 0) const
	{
		return ParseLocation(m_filePath, 1, locationId, 1, locationId);
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
		NameHierarchy nameHierarchy(NAME_DELIMITER_CXX);
		for (std::string element: utility::splitToVector(s, nameDelimiterTypeToString(NAME_DELIMITER_CXX)))
		{
			nameHierarchy.push(std::make_shared<NameElement>(element, NameElement::Signature()));
		}
		return nameHierarchy;
	}

	FilePath m_filePath;
};
