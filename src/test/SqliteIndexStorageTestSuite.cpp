#include "catch.hpp"

#include "FileSystem.h"
#include "SqliteIndexStorage.h"

TEST_CASE("storage adds node successfully")
{
	FilePath databasePath(L"data/SQLiteTestSuite/test.sqlite");
	int nodeCount = -1;
	{
		SqliteIndexStorage storage(databasePath);
		storage.setup();
		storage.beginTransaction();
		storage.addNode(StorageNodeData(0, L"a"));
		storage.commitTransaction();
		nodeCount = storage.getNodeCount();
	}
	FileSystem::remove(databasePath);

	REQUIRE(1 == nodeCount);
}

TEST_CASE("storage removes node successfully")
{
	FilePath databasePath(L"data/SQLiteTestSuite/test.sqlite");
	int nodeCount = -1;
	{
		SqliteIndexStorage storage(databasePath);
		storage.setup();
		storage.beginTransaction();
		Id nodeId = storage.addNode(StorageNodeData(0, L"a"));
		storage.removeElement(nodeId);
		storage.commitTransaction();
		nodeCount = storage.getNodeCount();
	}
	FileSystem::remove(databasePath);

	REQUIRE(0 == nodeCount);
}

TEST_CASE("storage adds edge successfully")
{
	FilePath databasePath(L"data/SQLiteTestSuite/test.sqlite");
	int edgeCount = -1;
	{
		SqliteIndexStorage storage(databasePath);
		storage.setup();
		storage.beginTransaction();
		Id sourceNodeId = storage.addNode(StorageNodeData(0, L"a"));
		Id targetNodeId = storage.addNode(StorageNodeData(0, L"b"));
		storage.addEdge(StorageEdgeData(0, sourceNodeId, targetNodeId));
		storage.commitTransaction();
		edgeCount = storage.getEdgeCount();
	}
	FileSystem::remove(databasePath);

	REQUIRE(1 == edgeCount);
}

TEST_CASE("storage removes edge successfully")
{
	FilePath databasePath(L"data/SQLiteTestSuite/test.sqlite");
	int edgeCount = -1;
	{
		SqliteIndexStorage storage(databasePath);
		storage.setup();
		storage.beginTransaction();
		Id sourceNodeId = storage.addNode(StorageNodeData(0, L"a"));
		Id targetNodeId = storage.addNode(StorageNodeData(0, L"b"));
		Id edgeId = storage.addEdge(StorageEdgeData(0, sourceNodeId, targetNodeId));
		storage.removeElement(edgeId);
		storage.commitTransaction();
		edgeCount = storage.getEdgeCount();
	}
	FileSystem::remove(databasePath);

	REQUIRE(0 == edgeCount);
}
