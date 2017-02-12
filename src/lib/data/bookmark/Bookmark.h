#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <vector>
#include <string>

#include "utility/TimePoint.h"
#include "utility/types.h"

#include "BookmarkCategory.h"

class Bookmark
{
public:
	Bookmark();
	Bookmark(const std::string& displayName, const std::vector<Id>& tokens, const std::vector<std::string>& tokenNames, const std::string& comment, const TimePoint& timeStamp);
	virtual ~Bookmark();

	Id getId() const;
	void setId(const Id id);

	std::vector<int> getTokenTypes() const;
	void setTokenTypes(const std::vector<int>& types);

	std::vector<Id> getTokenIds() const;
	void setTokenIds(const std::vector<Id>& ids);

	std::vector<std::string> getTokenNames() const;
	void setTokenNames(const std::vector<std::string>& names);

	std::string getComment() const;
	void setComment(const std::string& comment);

	std::string getDisplayName() const;
	void setDisplayName(const std::string& name);

	bool isValid() const;
	void setValid(const bool valid);

	TimePoint getTimeStamp() const;

	BookmarkCategory getCategory() const;
	void setCategory(const BookmarkCategory& category);

private:
	Id m_id;
	std::vector<int> m_tokenTypes;
	std::vector<Id> m_tokenIds;
	std::vector<std::string> m_tokenNames;
	std::string m_comment;
	std::string m_displayName;

	bool m_valid;

	TimePoint m_timeStamp;

	BookmarkCategory m_category;
};

#endif // BOOKMARK_H