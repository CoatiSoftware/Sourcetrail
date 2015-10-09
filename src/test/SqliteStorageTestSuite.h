#include "cxxtest/TestSuite.h"

#include "boost/filesystem.hpp"

#include "sqlite/CppSQLite3.h"
#include "data/SqliteStorage.h"

class SqliteStorageTestSuite: public CxxTest::TestSuite
{
public:
	void test_storage_adds_name_hierarchy_element_successfully()
	{
		std::string databasePath = "data/SQLiteTestSuite/test.sqlite";
		int elementCount = -1;
		{
			SqliteStorage storage(databasePath);
			storage.beginTransaction();
			storage.addNameHierarchyElement("a");
			storage.commitTransaction();
			elementCount = storage.getNameHierarchyElementCount();
		}
		boost::filesystem::remove(databasePath);

		TS_ASSERT_EQUALS(1, elementCount);
	}

	void test_storage_removes_name_hierarchy_element_successfully()
	{
		std::string databasePath = "data/SQLiteTestSuite/test.sqlite";
		int elementCount = -1;
		{
			SqliteStorage storage(databasePath);
			storage.beginTransaction();
			int elementId = storage.addNameHierarchyElement("a");
			storage.removeNameHierarchyElement(elementId);
			storage.commitTransaction();
			elementCount = storage.getNameHierarchyElementCount();
		}
		boost::filesystem::remove(databasePath);

		TS_ASSERT_EQUALS(0, elementCount);
	}

	void test_storage_finds_name_hierarchy_elements_by_name()
	{
		std::string databasePath = "data/SQLiteTestSuite/test.sqlite";
		int insertedElementId = -1;
		int foundElementId = -1;
		{
			SqliteStorage storage(databasePath);
			storage.beginTransaction();
			insertedElementId = storage.addNameHierarchyElement("a");
			storage.commitTransaction();
			foundElementId = storage.getNameHierarchyElementIdByName("a");
		}
		boost::filesystem::remove(databasePath);

		TS_ASSERT(foundElementId != -1);
		TS_ASSERT_EQUALS(insertedElementId, foundElementId);
	}

	void test_storage_finds_name_hierarchy_elements_by_name_and_parent_id()
	{
		std::string databasePath = "data/SQLiteTestSuite/test.sqlite";
		int insertedChildElementId = -1;
		int foundElementId = -1;
		{
			SqliteStorage storage(databasePath);
			storage.beginTransaction();
			int insertedParentElementId = storage.addNameHierarchyElement("a");
			insertedChildElementId = storage.addNameHierarchyElement("a", insertedParentElementId);
			storage.addNameHierarchyElement("a", storage.addNameHierarchyElement("b"));
			storage.commitTransaction();
			foundElementId = storage.getNameHierarchyElementIdByName("a", insertedParentElementId);
		}
		boost::filesystem::remove(databasePath);

		TS_ASSERT(foundElementId != -1);
		TS_ASSERT_EQUALS(insertedChildElementId, foundElementId);
	}

	void test_storage_automatically_removes_name_hierarchy_element_child_when_deleting_parent()
	{
		std::string databasePath = "data/SQLiteTestSuite/test.sqlite";
		int elementCount = -1;
		{
			SqliteStorage storage(databasePath);
			storage.beginTransaction();
			int parentId = storage.addNameHierarchyElement("a");
			storage.removeNameHierarchyElement(parentId);
			storage.commitTransaction();
			elementCount = storage.getNameHierarchyElementCount();
		}
		boost::filesystem::remove(databasePath);

		TS_ASSERT_EQUALS(0, elementCount);
	}

	void test_storage_adds_node_successfully()
	{
		std::string databasePath = "data/SQLiteTestSuite/test.sqlite";
		int nodeCount = -1;
		{
			SqliteStorage storage(databasePath);
			storage.beginTransaction();
			int nameId = storage.addNameHierarchyElement("a");
			storage.addNode(0, nameId, false);
			storage.commitTransaction();
			nodeCount = storage.getNodeCount();
		}
		boost::filesystem::remove(databasePath);

		TS_ASSERT_EQUALS(1, nodeCount);
	}

	void test_storage_removes_node_successfully()
	{
		std::string databasePath = "data/SQLiteTestSuite/test.sqlite";
		int nodeCount = -1;
		{
			SqliteStorage storage(databasePath);
			storage.beginTransaction();
			int nameId = storage.addNameHierarchyElement("a");
			int nodeId = storage.addNode(0, nameId, false);
			storage.removeElement(nodeId);
			storage.commitTransaction();
			nodeCount = storage.getNodeCount();
		}
		boost::filesystem::remove(databasePath);

		TS_ASSERT_EQUALS(0, nodeCount);
	}

	void test_storage_adds_edge_successfully()
	{
		std::string databasePath = "data/SQLiteTestSuite/test.sqlite";
		int edgeCount = -1;
		{
			SqliteStorage storage(databasePath);
			storage.beginTransaction();
			int sourceNameId = storage.addNameHierarchyElement("a");
			int sourceNodeId = storage.addNode(0, sourceNameId, false);
			int targetNameId = storage.addNameHierarchyElement("b");
			int targetNodeId = storage.addNode(0, targetNameId, false);
			storage.addEdge(0, sourceNodeId, targetNodeId);
			storage.commitTransaction();
			edgeCount = storage.getEdgeCount();
		}
		boost::filesystem::remove(databasePath);

		TS_ASSERT_EQUALS(1, edgeCount);
	}

	void test_storage_removes_edge_successfully()
	{
		std::string databasePath = "data/SQLiteTestSuite/test.sqlite";
		int edgeCount = -1;
		{
			SqliteStorage storage(databasePath);
			storage.beginTransaction();
			int sourceNameId = storage.addNameHierarchyElement("a");
			int sourceNodeId = storage.addNode(0, sourceNameId, false);
			int targetNameId = storage.addNameHierarchyElement("b");
			int targetNodeId = storage.addNode(0, targetNameId, false);
			int edgeId = storage.addEdge(0, sourceNodeId, targetNodeId);
			storage.removeElement(edgeId);
			storage.commitTransaction();
			edgeCount = storage.getEdgeCount();
		}
		boost::filesystem::remove(databasePath);

		TS_ASSERT_EQUALS(0, edgeCount);
	}
};
