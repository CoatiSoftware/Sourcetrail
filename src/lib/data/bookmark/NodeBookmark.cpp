#include "NodeBookmark.h"

NodeBookmark::NodeBookmark()
	: Bookmark()
{
}

NodeBookmark::NodeBookmark(const std::string& displayName, const std::vector<Id>& tokens, const std::vector<std::string>& tokenNames, const std::string& comment, const TimePoint& timeStamp)
	: Bookmark(displayName, tokens, tokenNames, comment, timeStamp)
{
}

NodeBookmark::~NodeBookmark()
{
}