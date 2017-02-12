#include "EdgeBookmark.h"

EdgeBookmark::EdgeBookmark()
	: Bookmark()
{
}

EdgeBookmark::EdgeBookmark(const std::string& displayName, const std::vector<Id>& tokens, const std::vector<std::string>& tokenNames, const std::string& comment, const TimePoint& timeStamp)
	: Bookmark(displayName, tokens, tokenNames, comment, timeStamp)
{
}

EdgeBookmark::~EdgeBookmark()
{
}

std::vector<int> EdgeBookmark::getEdgeTokenTypes() const
{
	return m_edgeTokenTypes;
}

void EdgeBookmark::setEdgeTokenTypes(const std::vector<int>& tokenTypes)
{
	m_edgeTokenTypes = tokenTypes;
}

std::vector<Id> EdgeBookmark::getEdgeTokenIds() const
{
	return m_edgeTokenIds;
}

void EdgeBookmark::setEdgeTokenIds(const std::vector<Id>& tokenIds)
{
	m_edgeTokenIds = tokenIds;
}

std::vector<std::string> EdgeBookmark::getEdgeTokenNames() const
{
	return m_edgeTokenNames;
}

void EdgeBookmark::setEdgeTokenNames(const std::vector<std::string>& tokenNames)
{
	m_edgeTokenNames = tokenNames;
}

NodeBookmark EdgeBookmark::getBaseBookmark() const
{
	return m_baseBookmark;
}

void EdgeBookmark::setBaseBookmark(const NodeBookmark& baseBookmark)
{
	m_baseBookmark = baseBookmark;
}