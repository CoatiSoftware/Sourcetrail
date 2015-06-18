#include "data/StorageCache.h"

void StorageCache::clear()
{
	m_queryToTokenIds.clear();
}

std::vector<Id> StorageCache::getTokenIdsForQuery(std::string query) const
{
	std::map<std::string, std::vector<Id>>::const_iterator it = m_queryToTokenIds.find(query);

	if (it != m_queryToTokenIds.end())
	{
		return it->second;
	}

	std::vector<Id> ids = StorageAccessProxy::getTokenIdsForQuery(query);

	m_queryToTokenIds.emplace(query, ids);

	return ids;
}
