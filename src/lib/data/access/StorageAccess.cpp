#include "data/access/StorageAccess.h"

std::pair<std::vector<Id>, std::vector<SearchMatch>> StorageAccess::getNodeIdsAndSearchMatchesForNameHierarchies(
	const std::vector<NameHierarchy> nameHierarchies) const
{
	std::vector<Id> tokenIds = getNodeIdsForNameHierarchies(nameHierarchies);
	std::vector<SearchMatch> matches;

	if (tokenIds.size())
	{
		matches = getSearchMatchesForTokenIds(tokenIds);
	}
	else
	{
		for (const NameHierarchy& name : nameHierarchies)
		{
			matches.push_back(SearchMatch(name.getQualifiedName()));
		}

		if (!matches.size())
		{
			matches.push_back(SearchMatch(L"<invalid>"));
		}
	}

	return std::make_pair(tokenIds, matches);
}
