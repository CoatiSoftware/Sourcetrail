#ifndef NODE_BOOKMARK_H
#define NODE_BOOKMARK_H

#include "Bookmark.h"

class NodeBookmark
	: public Bookmark
{
public:
	NodeBookmark();
	NodeBookmark(const std::string& displayName, const std::vector<Id>& tokens, const std::vector<std::string>& tokenNames, const std::string& comment, const TimePoint& timeStamp);
	virtual ~NodeBookmark();
};

#endif // NODE_BOOKMARK_H
