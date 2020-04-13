#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <string>
#include <vector>

#include "../../utility/TimeStamp.h"
#include "../../utility/types.h"

#include "BookmarkCategory.h"

class Bookmark
{
public:
	enum BookmarkFilter
	{
		FILTER_UNKNOWN = 0,
		FILTER_ALL,
		FILTER_NODES,
		FILTER_EDGES
	};

	enum BookmarkOrder
	{
		ORDER_NONE = 0,
		ORDER_DATE_ASCENDING,
		ORDER_DATE_DESCENDING,
		ORDER_NAME_ASCENDING,
		ORDER_NAME_DESCENDING
	};

	Bookmark(
		const Id id,
		const std::wstring& name,
		const std::wstring& comment,
		const TimeStamp& timeStamp,
		const BookmarkCategory& category);
	virtual ~Bookmark();

	Id getId() const;
	void setId(const Id id);

	std::wstring getName() const;
	void setName(const std::wstring& name);

	std::wstring getComment() const;
	void setComment(const std::wstring& comment);

	TimeStamp getTimeStamp() const;
	void setTimeStamp(const TimeStamp& timeStamp);

	BookmarkCategory getCategory() const;
	void setCategory(const BookmarkCategory& category);

	bool isValid() const;
	void setIsValid(const bool isValid = true);

private:
	Id m_id;
	std::wstring m_name;
	std::wstring m_comment;
	TimeStamp m_timeStamp;
	BookmarkCategory m_category;
	bool m_isValid;
};

#endif	  // BOOKMARK_H
