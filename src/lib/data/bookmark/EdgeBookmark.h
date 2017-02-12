#ifndef EDGE_BOOKMARK_H
#define EDGE_BOOKMARK_H

#include "Bookmark.h"
#include "NodeBookmark.h"

class EdgeBookmark
	: public Bookmark
{
public:
	EdgeBookmark();
	EdgeBookmark(const std::string& displayName, const std::vector<Id>& tokens, const std::vector<std::string>& tokenNames, const std::string& comment, const TimePoint& timeStamp);
	virtual ~EdgeBookmark();

	std::vector<int> getEdgeTokenTypes() const;
	void setEdgeTokenTypes(const std::vector<int>& tokenTypes);

	std::vector<Id> getEdgeTokenIds() const;
	void setEdgeTokenIds(const std::vector<Id>& tokenIds);

	std::vector<std::string> getEdgeTokenNames() const;
	void setEdgeTokenNames(const std::vector<std::string>& tokenNames);

	NodeBookmark getBaseBookmark() const;
	void setBaseBookmark(const NodeBookmark& baseBookmark);

private:
	std::vector<int> m_edgeTokenTypes;
	std::vector<Id> m_edgeTokenIds;
	std::vector<std::string> m_edgeTokenNames;

	NodeBookmark m_baseBookmark;
};

#endif // EDGE_BOOKMARK_H