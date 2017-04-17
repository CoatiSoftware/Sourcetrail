#ifndef EDGE_BOOKMARK_H
#define EDGE_BOOKMARK_H

#include "Bookmark.h"
#include "data/graph/Edge.h"

class EdgeBookmark
	: public Bookmark
{
public:
	EdgeBookmark(const Id id, const std::string& name, const std::string& comment, const TimePoint& timeStamp, const BookmarkCategory& category);
	virtual ~EdgeBookmark();

	void addEdgeId(const Id edgeId);
	void setEdgeIds(const std::vector<Id>& edgesIds);
	std::vector<Id> getEdgeIds() const;

	void setActiveNodeId(const Id activeNodeId);
	Id getActiveNodeId() const;

private:
	std::vector<Id> m_edgeIds;
	Id m_activeNodeId;
};

#endif // EDGE_BOOKMARK_H
