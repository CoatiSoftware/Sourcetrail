#ifndef STORAGE_BOOKMARK_H
#define STORAGE_BOOKMARK_H

#include <string>

#include "../../../utility/types.h"

struct StorageBookmarkData
{
	StorageBookmarkData(): name(L""), comment(L""), timestamp(""), categoryId(0) {}

	StorageBookmarkData(
		const std::wstring& name,
		const std::wstring& comment,
		const std::string& timestamp,
		const Id categoryId)
		: name(name), comment(comment), timestamp(timestamp), categoryId(categoryId)
	{
	}

	std::wstring name;
	std::wstring comment;
	std::string timestamp;
	Id categoryId;
};

struct StorageBookmark: public StorageBookmarkData
{
	StorageBookmark(): StorageBookmarkData(), id(0) {}

	StorageBookmark(Id id, const StorageBookmarkData& data): StorageBookmarkData(data), id(id) {}

	StorageBookmark(
		Id id,
		const std::wstring& name,
		const std::wstring& comment,
		const std::string& timestamp,
		const Id categoryId)
		: StorageBookmarkData(name, comment, timestamp, categoryId), id(id)
	{
	}

	Id id;
};

#endif	  // STORAGE_BOOKMARK_H
