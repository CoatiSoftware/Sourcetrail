#ifndef SEARCH_MATCH_H
#define SEARCH_MATCH_H

#include <deque>
#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "data/graph/Node.h"
#include "data/query/QueryNode.h"
#include "utility/types.h"

struct SearchMatch
{
	static void log(const std::vector<SearchMatch>& matches, const std::string& query);
	static std::deque<SearchMatch> stringDequeToSearchMatchDeque(const std::deque<std::string>& deque);
	static std::deque<std::string> searchMatchDequeToStringDeque(const std::deque<SearchMatch>& deque);
	static std::string searchMatchDequeToString(const std::deque<SearchMatch>& deque);

	SearchMatch();
	SearchMatch(const std::string& query);

	void print(std::ostream& ostream) const;

	std::string encodeForQuery() const;

	void decodeFromQuery(std::string query);
	std::string getNodeTypeAsString() const;

	std::string fullName;
	std::string typeName;
	Node::NodeType nodeType;
	QueryNode::QueryNodeType queryNodeType;

	std::set<Id> tokenIds;
	std::vector<size_t> indices;
	size_t weight;
};


#endif // SEARCH_MATCH_H
