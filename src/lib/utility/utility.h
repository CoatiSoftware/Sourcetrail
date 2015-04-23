#ifndef UTILITY_H
#define UTILITY_H

#include <chrono>
#include <set>

#include "utility/math/Vector2.h"

namespace utility
{
	typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

	TimePoint durationStart();
	float duration(const TimePoint& start);
	float duration(std::function<void()> func);

	template<typename T>
	std::vector<T> concat(const std::vector<T>& a, const std::vector<T>& b);

	template<typename T>
	void append(std::vector<T>& a, const std::vector<T>& b);

	template<typename T>
	void append(std::set<T>& a, const std::set<T>& b);

	bool intersectionPoint(Vec2f a1, Vec2f b1, Vec2f a2, Vec2f b2, Vec2f* i);
}

template<typename T>
std::vector<T> utility::concat(const std::vector<T>& a, const std::vector<T>& b)
{
	std::vector<T> r(a.size() + b.size());
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

#endif // UTILITY_H
