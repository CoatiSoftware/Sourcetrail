#include "cxxtest/TestSuite.h"

#include "boost/filesystem.hpp"

#include "data/storage/sqlite/SqliteIndexStorage.h"

class SqliteIndexStorageTestSuite: public CxxTest::TestSuite
{
public:
	void test_storage_adds_node_successfully()
	{
		FilePath databasePath("data/SQLiteTestSuite/test.sqlite");
		int nodeCount = -1;
		{
			SqliteIndexStorage storage(databasePath);
			storage.setup();
			storage.beginTransaction();
			storage.addNode(StorageNodeData(0, "a"));
			storage.commitTransaction();
			nodeCount = storage.getNodeCount();
		}
		boost::filesystem::remove(databasePath.path());

		TS_ASSERT_EQUALS(1, nodeCount);
	}

	void test_storage_removes_node_successfully()
	{
		FilePath databasePath("data/SQLiteTestSuite/test.sqlite");
		int nodeCount = -1;
		{
			SqliteIndexStorage storage(databasePath);
			storage.setup();
			storage.beginTransaction();
			int nodeId = storage.addNode(StorageNodeData(0, "a")).id;
			storage.removeElement(nodeId);
			storage.commitTransaction();
			nodeCount = storage.getNodeCount();
		}
		boost::filesystem::remove(databasePath.path());

		TS_ASSERT_EQUALS(0, nodeCount);
	}

	void test_storage_adds_edge_successfully()
	{
		FilePath databasePath("data/SQLiteTestSuite/test.sqlite");
		int edgeCount = -1;
		{
			SqliteIndexStorage storage(databasePath);
			storage.setup();
			storage.beginTransaction();
			int sourceNodeId = storage.addNode(StorageNodeData(0, "a")).id;
			int targetNodeId = storage.addNode(StorageNodeData(0, "b")).id;
			storage.addEdge(StorageEdgeData(0, sourceNodeId, targetNodeId));
			storage.commitTransaction();
			edgeCount = storage.getEdgeCount();
		}
		boost::filesystem::remove(databasePath.path());

		TS_ASSERT_EQUALS(1, edgeCount);
	}

	void test_storage_removes_edge_successfully()
	{
		FilePath databasePath("data/SQLiteTestSuite/test.sqlite");
		int edgeCount = -1;
		{
			SqliteIndexStorage storage(databasePath);
			storage.setup();
			storage.beginTransaction();
			int sourceNodeId = storage.addNode(StorageNodeData(0, "a")).id;
			int targetNodeId = storage.addNode(StorageNodeData(0, "b")).id;
			int edgeId = storage.addEdge(StorageEdgeData(0, sourceNodeId, targetNodeId)).id;
			storage.removeElement(edgeId);
			storage.commitTransaction();
			edgeCount = storage.getEdgeCount();
		}
		boost::filesystem::remove(databasePath.path());

		TS_ASSERT_EQUALS(0, edgeCount);
	}
};
