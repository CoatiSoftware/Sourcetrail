#ifndef CACHE_H
#define CACHE_H

#include <functional>
#include <unordered_map>

template <typename KeyType, typename ValType, typename Hasher = std::hash<KeyType>>
class Cache
{
public:
	Cache(std::function<ValType(KeyType)> calculator);
	ValType getValue(KeyType key);

private:
	std::function<ValType(KeyType)> m_calculator;
	std::unordered_map<KeyType, ValType, Hasher> m_map;

	size_t m_hitCount;
	size_t m_missCount;
};

template <typename KeyType, typename ValType, typename Hasher>
Cache<KeyType, ValType, Hasher>::Cache(std::function<ValType(KeyType)> calculator)
	: m_calculator(calculator)
	, m_hitCount(0)
	, m_missCount(0)
{
}

template <typename KeyType, typename ValType, typename Hasher>
ValType Cache<KeyType, ValType, Hasher>::getValue(KeyType key)
{
	typename std::unordered_map<KeyType, ValType>::const_iterator it = m_map.find(key);
	if (it != m_map.end())
	{
		++m_hitCount;
		return it->second;
	}
	++m_missCount;
	ValType val = m_calculator(key);
	m_map[key] = val;
	return val;
}

#endif // CACHE_H
