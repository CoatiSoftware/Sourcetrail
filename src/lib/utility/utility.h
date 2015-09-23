#ifndef UTILITY_H
#define UTILITY_H

#include <deque>
#include <chrono>
#include <set>
#include <time.h>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "utility/math/Vector2.h"

namespace utility
{
	typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

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
	std::vector<T> toVector(const std::deque<T>& d);

	template<typename T>
	std::vector<std::string> toStrings(const std::vector<T>& d);

	bool intersectionPoint(Vec2f a1, Vec2f b1, Vec2f a2, Vec2f b2, Vec2f* i);

	size_t digits(size_t n);
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
	a.insert(a.begin(), b.begin(), b.end());
}

template<typename T>
void utility::append(std::set<T>& a, const std::set<T>& b)
{
	a.insert(b.begin(), b.end());
}

template<typename T>
std::vector<T> utility::toVector(const std::deque<T>& d)
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

#endif // UTILITY_H
