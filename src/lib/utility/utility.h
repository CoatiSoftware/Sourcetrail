#ifndef UTILITY_H
#define UTILITY_H

#include <algorithm>
#include <deque>
#include <set>
#include <time.h>
#include <unordered_set>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "utility/file/FilePath.h"
#include "utility/math/Vector2.h"
#include "utility/TimePoint.h"

namespace utility
{
	TimePoint durationStart();
	float duration(const TimePoint& start);
	float duration(std::function<void()> func);

	std::string timeToString(const time_t time);
	std::string timeToString(const boost::posix_time::ptime time);

	template<typename T>
	std::vector<T> concat(const std::vector<T>& a, const std::vector<T>& b);

	template<typename T>
	void append(std::vector<T>& a, const std::vector<T>& b);

	template<typename T>
	void append(std::set<T>& a, const std::set<T>& b);

	template<typename T>
	void append(std::unordered_set<T>& a, const std::unordered_set<T>& b);

	template<typename T>
	std::vector<T> unique(const std::vector<T>& a);

	template<typename T>
	std::vector<T> toVector(const std::deque<T>& d);

	template<typename T>
	std::vector<T> toVector(const std::set<T>& d);

	template<typename T>
	std::vector<std::string> toStrings(const std::vector<T>& d);

	template<typename T>
	bool isPermutation(const std::vector<T>& a, const std::vector<T>& b)
	{
		return (
			a.size() == b.size() &&
			std::is_permutation(a.begin(), a.end(), b.begin())
		);
	}

	bool intersectionPoint(Vec2f a1, Vec2f b1, Vec2f a2, Vec2f b2, Vec2f* i);

	size_t digits(size_t n);

	int roundToInt(float n);
}

template<typename T>
std::vector<T> utility::concat(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> r;
	append(r, a);
	append(r, b);
	return r;
}

template<typename T>
void utility::append(std::vector<T>& a, const std::vector<T>& b)
{
	a.insert(a.end(), b.begin(), b.end());
}

template<typename T>
void utility::append(std::set<T>& a, const std::set<T>& b)
{
	a.insert(b.begin(), b.end());
}

template<typename T>
void utility::append(std::unordered_set<T>& a, const std::unordered_set<T>& b)
{
	a.insert(b.begin(), b.end());
}

template<typename T>
std::vector<T> utility::unique(const std::vector<T>& a)
{
	std::map<T, size_t> unique;

	size_t i = 0;
	for (const T& t : a)
	{
		if (unique.emplace(t, i).second)
		{
			i++;
		}
	}

	std::vector<T> r(i, T());
	for (const std::pair<T, size_t>& p : unique)
	{
		r[p.second] = p.first;
	}

	return r;
}

template<typename T>
std::vector<T> utility::toVector(const std::deque<T>& d)
{
	std::vector<T> v;
	v.insert(v.begin(), d.begin(), d.end());
	return v;
}

template<typename T>
std::vector<T> utility::toVector(const std::set<T>& d)
{
	std::vector<T> v;
	v.insert(v.begin(), d.begin(), d.end());
	return v;
}

template<typename T>
std::vector<std::string> utility::toStrings(const std::vector<T>& d)
{
	std::vector<std::string> v;
	for (const T& t : d)
	{
		v.push_back(std::to_string(t));
	}
	return v;
}

template<>
inline std::vector<std::string> utility::toStrings<FilePath>(const std::vector<FilePath>& d)
{
	std::vector<std::string> v;
	for (const FilePath& t : d)
	{
		v.push_back(t.str());
	}
	return v;
}

#endif // UTILITY_H
