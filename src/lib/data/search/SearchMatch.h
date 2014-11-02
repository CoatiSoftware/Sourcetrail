#ifndef SEARCH_MATCH_H
#define SEARCH_MATCH_H

#include <ostream>
#include <set>
#include <string>
#include <vector>

#include "utility/types.h"

struct SearchMatch
{
	static void log(const std::vector<SearchMatch>& matches, const std::string& query);
	void print(std::ostream& ostream) const;

	std::string encodeForQuery() const;

	std::string fullName;
	std::string typeName;
	std::set<Id> tokenIds;
	std::vector<size_t> indices;
	size_t weight;
};

#endif // SEARCH_MATCH_H
