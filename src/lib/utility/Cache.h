#ifndef CACHE_H
#define CACHE_H

#include <functional>
#include <map>

template <typename KeyType, typename ValType>
class Cache
{
public:
	Cache(std::function<ValType(KeyType)> calculator);
	ValType getValue(KeyType key);

private:
	std::function<ValType(KeyType)> m_calculator;
	std::map<KeyType, ValType> m_map;
};

template <typename KeyType, typename ValType>
Cache<KeyType, ValType>::Cache(std::function<ValType(KeyType)> calculator)
	: m_calculator(calculator)
{
}

template <typename KeyType, typename ValType>
ValType Cache<KeyType, ValType>::getValue(KeyType key)
{
	typename std::map<KeyType, ValType>::const_iterator it = m_map.find(key);
	if (it != m_map.end())
	{
		return it->second;
	}
	ValType val = m_calculator(key);
	m_map[key] = val;
	return val;
}

#endif // CACHE_H
