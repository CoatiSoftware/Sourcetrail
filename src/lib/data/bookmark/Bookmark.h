#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <vector>
#include <string>

#include "utility/TimeStamp.h"
#include "utility/types.h"

#include "BookmarkCategory.h"

class Bookmark
{
public:
	Bookmark(const Id id, const std::string& name, const std::string& comment, const TimeStamp& timeStamp, const BookmarkCategory& category);
	virtual ~Bookmark();

	Id getId() const;
	void setId(const Id id);

	std::string getName() const;
	void setName(const std::string& name);

	std::string getComment() const;
	void setComment(const std::string& comment);

	TimeStamp getTimeStamp() const;
	void setTimeStamp(const TimeStamp& timeStamp);

	BookmarkCategory getCategory() const;
	void setCategory(const BookmarkCategory& category);

	bool isValid() const;
	void setIsValid(const bool isValid = true);

private:
	Id m_id;
	std::string m_name;
	std::string m_comment;
	TimeStamp m_timeStamp;
	BookmarkCategory m_category;
	bool m_isValid;
};

#endif // BOOKMARK_H
