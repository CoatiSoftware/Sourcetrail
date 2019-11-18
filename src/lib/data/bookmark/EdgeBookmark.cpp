#include "EdgeBookmark.h"

EdgeBookmark::EdgeBookmark(
	const Id id,
	const std::wstring& name,
	const std::wstring& comment,
	const TimeStamp& timeStamp,
	const BookmarkCategory& category)
	: Bookmark(id, name, comment, timeStamp, category)
{
}

EdgeBookmark::~EdgeBookmark() {}

void EdgeBookmark::addEdgeId(const Id edgeId)
{
	m_edgeIds.push_back(edgeId);
}

void EdgeBookmark::setEdgeIds(const std::vector<Id>& edgesIds)
{
	m_edgeIds = edgesIds;
}

std::vector<Id> EdgeBookmark::getEdgeIds() const
{
	return m_edgeIds;
}

void EdgeBookmark::setActiveNodeId(const Id activeNodeId)
{
	m_activeNodeId = activeNodeId;
}

Id EdgeBookmark::getActiveNodeId() const
{
	return m_activeNodeId;
}
