#include <cxxtest/TestSuite.h>

#include "utilityString.h"

#include "ParseLocation.h"
#include "IntermediateStorage.h"
#include "PersistentStorage.h"

class StorageTestSuite: public CxxTest::TestSuite
{
public:
	void setUp()
	{
		m_filePath = FilePath(L"file.cpp");
	}

	void test_storage_saves_file()
	{
		TestStorage storage;

		std::wstring filePath = L"path/to/test.h";

		std::shared_ptr<IntermediateStorage> intermetiateStorage = std::make_shared<IntermediateStorage>();
		Id id = intermetiateStorage->addNode(StorageNodeData(NodeType::typeToInt(NodeType::NODE_FILE), NameHierarchy::serialize(NameHierarchy(filePath, NAME_DELIMITER_FILE)))).first;
		intermetiateStorage->addFile(StorageFile(id, filePath, "someTime", true, true));

		storage.inject(intermetiateStorage.get());

		TS_ASSERT_EQUALS(storage.getNameHierarchyForNodeId(id).getQualifiedName(), filePath);
		TS_ASSERT(storage.getNodeTypeForNodeWithId(id).isFile());

	}
	void test_storage_saves_node()
	{
		NameHierarchy a = createNameHierarchy(L"type");

		TestStorage storage;

		std::shared_ptr<IntermediateStorage> intermetiateStorage = std::make_shared<IntermediateStorage>();
		intermetiateStorage->addNode(StorageNodeData(NodeType::typeToInt(NodeType::NODE_TYPEDEF), NameHierarchy::serialize(a)));

		storage.inject(intermetiateStorage.get());

		Id storedId = storage.getNodeIdForNameHierarchy(a);

		TS_ASSERT(storedId != 0);
		TS_ASSERT_EQUALS(storage.getNodeTypeForNodeWithId(storedId).getType(), NodeType::NODE_TYPEDEF);
	}


	void test_storage_saves_field_as_member()
	{
		NameHierarchy a = createNameHierarchy(L"Struct");
		NameHierarchy b = createNameHierarchy(L"Struct::m_field");

		TestStorage storage;

		std::shared_ptr<IntermediateStorage> intermetiateStorage = std::make_shared<IntermediateStorage>();

		Id aId = intermetiateStorage->addNode(StorageNodeData(NodeType::typeToInt(NodeType::NODE_STRUCT), NameHierarchy::serialize(a))).first;
		intermetiateStorage->addSymbol(StorageSymbol(aId, DEFINITION_EXPLICIT));

		Id bId = intermetiateStorage->addNode(StorageNodeData(NodeType::typeToInt(NodeType::NODE_FIELD), NameHierarchy::serialize(b))).first;
		intermetiateStorage->addSymbol(StorageSymbol(bId, DEFINITION_EXPLICIT));
		intermetiateStorage->addEdge(StorageEdgeData(Edge::typeToInt(Edge::EDGE_MEMBER), aId, bId));

		storage.inject(intermetiateStorage.get());
		bool foundEdge = false;

		const Id sourceId = storage.getNodeIdForNameHierarchy(a);
		const Id targetId = storage.getNodeIdForNameHierarchy(b);
		for (auto edge : storage.getStorageEdges())
		{
			if (edge.sourceNodeId == sourceId && edge.targetNodeId == targetId && edge.type == Edge::typeToInt(Edge::EDGE_MEMBER))
			{
				foundEdge = true;
			}
		}
		TS_ASSERT(foundEdge);
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
		//TS_ASSERT_EQUALS(node->getType(), NodeType::NODE_METHOD);
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
			: PersistentStorage(FilePath(L"data/test.sqlite"), FilePath(L"data/testBookmarks.sqlite"))
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
	};

	ParseLocation validLocation(Id locationId = 0) const
	{
		return ParseLocation(1, 1, locationId, 1, locationId);
	}

	NameHierarchy createFunctionNameHierarchy(std::wstring ret, std::wstring name, std::wstring parameters) const
	{
		NameHierarchy nameHierarchy = createNameHierarchy(name);
		std::wstring lastName = nameHierarchy.back().getName();
		nameHierarchy.pop();
		nameHierarchy.push(NameElement(lastName, ret, parameters));
		return nameHierarchy;
	}

	NameHierarchy createNameHierarchy(std::wstring s) const
	{
		NameHierarchy nameHierarchy(NAME_DELIMITER_CXX);
		for (std::wstring element: utility::splitToVector(s, nameDelimiterTypeToString(NAME_DELIMITER_CXX)))
		{
			nameHierarchy.push(element);
		}
		return nameHierarchy;
	}

	FilePath m_filePath;
};
