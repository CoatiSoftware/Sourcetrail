#ifndef BOOKMARK_CATEGORY_H
#define BOOKMARK_CATEGORY_H

#include <string>

#include "utility/types.h"

class BookmarkCategory
{
public:
	BookmarkCategory();
	BookmarkCategory(const Id id, const std::string& name);
	~BookmarkCategory();

	Id getId() const;
	void setId(const Id id);

	std::string getName() const;
	void setName(const std::string& name);

private:
	Id m_id;
	std::string m_name;
};

#endif // BOOKMARK_CATEGORY_H
