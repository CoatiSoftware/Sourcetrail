#ifndef BOOKMARK_CATEGORY_H
#define BOOKMARK_CATEGORY_H

#include <string>

#include "../../utility/types.h"

class BookmarkCategory
{
public:
	BookmarkCategory();
	BookmarkCategory(const Id id, const std::wstring& name);
	~BookmarkCategory();

	Id getId() const;
	void setId(const Id id);

	std::wstring getName() const;
	void setName(const std::wstring& name);

private:
	Id m_id;
	std::wstring m_name;
};

#endif	  // BOOKMARK_CATEGORY_H
