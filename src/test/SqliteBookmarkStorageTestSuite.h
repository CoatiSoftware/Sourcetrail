#include "cxxtest/TestSuite.h"

#include "boost/filesystem.hpp"

#include "data/bookmark/EdgeBookmark.h"
#include "data/bookmark/NodeBookmark.h"
#include "data/SqliteBookmarkStorage.h"

class SqliteBookmarkStorageTestSuite: public CxxTest::TestSuite
{
public:
	void test_add_bookmarks()
	{
		FilePath databasePath("data/SQLiteTestSuite/bookmarkTest.sqlite");
		int bookmarkCount = 4;
		int result = -1;
		{
			boost::filesystem::remove(databasePath.path());
			SqliteBookmarkStorage storage(databasePath);
			storage.setup();

			for (unsigned int i = 0; i < bookmarkCount; i++)
			{
				const Id categoryId = storage.addBookmarkCategory("test category");
				storage.addBookmark("test bookmark", "test comment", TimePoint::now().toString(), categoryId);
			}

			result = storage.getAllBookmarks().size();
		}

		boost::filesystem::remove(databasePath.path());

		TS_ASSERT_EQUALS(result, bookmarkCount);
	}

	void test_add_bookmarked_node()
	{
		FilePath databasePath("data/SQLiteTestSuite/bookmarkTest.sqlite");
		int bookmarkCount = 4;
		int result = -1;
		{
			boost::filesystem::remove(databasePath.path());
			SqliteBookmarkStorage storage(databasePath);
			storage.setup();

			const Id categoryId = storage.addBookmarkCategory("test category");
			const Id bookmarkId = storage.addBookmark("test bookmark", "test comment", TimePoint::now().toString(), categoryId);

			for (unsigned int i = 0; i < bookmarkCount; i++)
			{
				storage.addBookmarkedNode(bookmarkId, "test name");
			}

			result = storage.getAllBookmarkedNodes().size();
		}

		boost::filesystem::remove(databasePath.path());

		TS_ASSERT_EQUALS(result, bookmarkCount);
	}

	void test_remove_bookmark_also_removes_bookmarked_node()
	{
		FilePath databasePath("data/SQLiteTestSuite/bookmarkTest.sqlite");
		int bookmarkCount = 4;
		int result = -1;
		{
			boost::filesystem::remove(databasePath.path());
			SqliteBookmarkStorage storage(databasePath);
			storage.setup();

			const Id categoryId = storage.addBookmarkCategory("test category");
			const Id bookmarkId = storage.addBookmark("test bookmark", "test comment", TimePoint::now().toString(), categoryId);
			const Id bookmarkedNodeId = storage.addBookmarkedNode(bookmarkId, "test name");

			storage.removeBookmark(bookmarkId);

			result = storage.getAllBookmarkedNodes().size();
		}

		boost::filesystem::remove(databasePath.path());

		TS_ASSERT_EQUALS(result, 0);
	}

	void test_edit_nodeBookmark()
	{
		FilePath databasePath("data/SQLiteTestSuite/bookmarkTest.sqlite");

		const std::string updatedName = "updated name";
		const std::string updatedComment = "updated comment";

		StorageBookmark storageBookmark;
		{
			boost::filesystem::remove(databasePath.path());
			SqliteBookmarkStorage storage(databasePath);
			storage.setup();

			const Id categoryId = storage.addBookmarkCategory("test category");
			const Id bookmarkId = storage.addBookmark("test bookmark", "test comment", TimePoint::now().toString(), categoryId);
			const Id bookmarkedNodeId = storage.addBookmarkedNode(bookmarkId, "test name");

			storage.updateBookmark(bookmarkId, updatedName, updatedComment, categoryId);

			storageBookmark = storage.getAllBookmarks().front();
		}

		boost::filesystem::remove(databasePath.path());

		TS_ASSERT_EQUALS(updatedName, storageBookmark.name);
		TS_ASSERT_EQUALS(updatedComment, storageBookmark.comment);
	}
};
