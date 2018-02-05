#include "NodeBookmark.h"

NodeBookmark::NodeBookmark(const Id id, const std::wstring& name, const std::wstring& comment,
	const TimeStamp& timeStamp, const BookmarkCategory& category
)
	: Bookmark(id, name, comment, timeStamp, category)
{
}

NodeBookmark::~NodeBookmark()
{
}

void NodeBookmark::addNodeId(const Id nodeId)
{
	m_nodeIds.push_back(nodeId);
}

void NodeBookmark::setNodeIds(const std::vector<Id>& nodeIds)
{
	m_nodeIds = nodeIds;
}

std::vector<Id> NodeBookmark::getNodeIds() const
{
	return m_nodeIds;
}
