#ifndef NODE_BOOKMARK_H
#define NODE_BOOKMARK_H

#include "Bookmark.h"

class NodeBookmark
	: public Bookmark
{
public:
	NodeBookmark(const Id id, const std::string& name, const std::string& comment, const TimePoint& timeStamp, const BookmarkCategory& category);
	virtual ~NodeBookmark();

	void addNodeId(const Id nodeId);
	void setNodeIds(const std::vector<Id>& nodeIds);
	std::vector<Id> getNodeIds() const;

private:
	std::vector<Id> m_nodeIds;
};

#endif // NODE_BOOKMARK_H
