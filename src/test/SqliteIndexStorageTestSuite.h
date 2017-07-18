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
			storage.addNode(0, "a");
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
			int nodeId = storage.addNode(0, "a");
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
			int sourceNodeId = storage.addNode(0, "a");
			int targetNodeId = storage.addNode(0, "b");
			storage.addEdge(0, sourceNodeId, targetNodeId);
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
			int sourceNodeId = storage.addNode(0, "a");
			int targetNodeId = storage.addNode(0, "b");
			int edgeId = storage.addEdge(0, sourceNodeId, targetNodeId);
			storage.removeElement(edgeId);
			storage.commitTransaction();
			edgeCount = storage.getEdgeCount();
		}
		boost::filesystem::remove(databasePath.path());

		TS_ASSERT_EQUALS(0, edgeCount);
	}
};
