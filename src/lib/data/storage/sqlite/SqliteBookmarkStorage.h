#ifndef SQLITE_BOOKMARK_STORAGE_H
#define SQLITE_BOOKMARK_STORAGE_H

#include "data/storage/sqlite/SqliteStorage.h"
#include "data/storage/type/StorageBookmark.h"
#include "data/storage/type/StorageBookmarkCategory.h"
#include "data/storage/type/StorageBookmarkedNode.h"
#include "data/storage/type/StorageBookmarkedEdge.h"
#include "utility/types.h"

class SqliteBookmarkStorage
	: public SqliteStorage
{
public:
	SqliteBookmarkStorage(const FilePath& dbFilePath);

	virtual size_t getStaticVersion() const;

	void migrateIfNecessary();

	StorageBookmarkCategory addBookmarkCategory(const StorageBookmarkCategoryData& data);
	StorageBookmark addBookmark(const StorageBookmarkData& data);
	StorageBookmarkedNode addBookmarkedNode(const StorageBookmarkedNodeData& data);
	StorageBookmarkedEdge addBookmarkedEdge(const StorageBookmarkedEdgeData data);

	void removeBookmarkCategory(Id id);
	void removeBookmark(const Id id);

	std::vector<StorageBookmark> getAllBookmarks() const;
	std::vector<StorageBookmarkedNode> getAllBookmarkedNodes() const;
	std::vector<StorageBookmarkedEdge> getAllBookmarkedEdges() const;

	void updateBookmark(const Id bookmarkId, const std::wstring& name, const std::wstring& comment, const Id categoryId);

	std::vector<StorageBookmarkCategory> getAllBookmarkCategories() const;
	StorageBookmarkCategory getBookmarkCategoryByName(const std::wstring& name) const;

private:
	static const size_t s_storageVersion;

	virtual std::vector<std::pair<int, SqliteDatabaseIndex>> getIndices() const;
	virtual void clearTables();
	virtual void setupTables();
	virtual void setupPrecompiledStatements();

	//void updateBookmarkMetaData(const BookmarkMetaData& metaData);

	template <typename ResultType>
	std::vector<ResultType> doGetAll(const std::string& query) const;

	template <typename ResultType>
	ResultType doGetFirst(const std::string& query) const
	{
		std::vector<ResultType> results = doGetAll<ResultType>(query + " LIMIT 1");
		if (results.size() > 0)
		{
			return results[0];
		}
		return ResultType();
	}
};

template <>
std::vector<StorageBookmarkCategory> SqliteBookmarkStorage::doGetAll<StorageBookmarkCategory>(const std::string& query) const;
template <>
std::vector<StorageBookmark> SqliteBookmarkStorage::doGetAll<StorageBookmark>(const std::string& query) const;
template <>
std::vector<StorageBookmarkedNode> SqliteBookmarkStorage::doGetAll<StorageBookmarkedNode>(const std::string& query) const;
template <>
std::vector<StorageBookmarkedEdge> SqliteBookmarkStorage::doGetAll<StorageBookmarkedEdge>(const std::string& query) const;

#endif // SQLITE_BOOKMARK_STORAGE_H
