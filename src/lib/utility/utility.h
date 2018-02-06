#ifndef UTILITY_H
#define UTILITY_H

#include <algorithm>
#include <cstdarg>
#include <deque>
#include <functional>
#include <set>
#include <time.h>
#include <vector>
#include <unordered_set>

#include "boost/date_time/posix_time/posix_time.hpp"

#include "utility/ApplicationArchitectureType.h"
#include "utility/file/FilePath.h"
#include "utility/math/Vector2.h"
#include "utility/TimeStamp.h"
#include "utility/utilityString.h"

namespace utility
{
	TimeStamp durationStart();
	float duration(const TimeStamp& start);
	float duration(std::function<void()> func);

	std::string timeToString(const time_t time);
	std::string timeToString(const boost::posix_time::ptime time);
	std::string timeToString(float seconds);

	template<typename T>
	std::vector<T> concat(const std::vector<T>& a, const std::vector<T>& b);

	template<typename T>
	std::set<T> concat(const std::set<T>& a, const std::set<T>& b);

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
	std::set<T>toSet(const std::vector<T>& d);

	template<typename T>
	void fillVectorWithElements(std::vector<T>& v, const T& arg);

	template<typename T, typename... Args>
	void fillVectorWithElements(std::vector<T>& v, const T& arg, const Args&... args);

	template<typename T, typename... Args>
	std::vector<T> createVectorFromElements(const Args&... args);

	template<typename SourceType, typename TargetType>
	std::vector<TargetType> convert(const std::vector<SourceType>& sourceContainer, std::function<TargetType(const SourceType&)> conversion);

	template<typename T>
	std::vector<std::string> toStrings(const std::vector<T>& d);
	template<>
	std::vector<std::string> toStrings(const std::vector<FilePath>& d);

	template<typename T>
	std::vector<std::wstring> toWStrings(const std::vector<T>& d);
	template<>
	std::vector<std::wstring> toWStrings(const std::vector<FilePath>& d);

	template<typename T>
	bool isPermutation(const std::vector<T>& a, const std::vector<T>& b)
	{
		return (
			a.size() == b.size() &&
			std::is_permutation(a.begin(), a.end(), b.begin())
		);
	}

	template<typename T>
	bool containsElement(const std::vector<T>& v, const T& e)
	{
		for (const T& ve: v)
		{
			if (ve == e)
			{
				return true;
			}
		}
		return false;
	}

	template<typename T>
	bool shareElement(const std::set<T>& a, const std::set<T>& b)
	{
		const std::set<T>* aPtr = a.size() > b.size() ? &a : &b;
		const std::set<T>* bPtr = aPtr == &a ? &b : &a;

		for (const T& bt : *bPtr)
		{
			if (aPtr->find(bt) != aPtr->end())
			{
				return true;
			}
		}

		return false;
	}

    ApplicationArchitectureType getApplicationArchitectureType();

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
std::set<T> utility::concat(const std::set<T>& a, const std::set<T>& b)
{
	std::set<T> r;
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
std::set<T> utility::toSet(const std::vector<T>& v)
{
	std::set<T> s(v.begin(), v.end());
	return s;
}

template<typename T>
void utility::fillVectorWithElements(std::vector<T>& v, const T& arg)
{
	v.push_back(arg);
}

template<typename T, typename... Args>
void utility::fillVectorWithElements(std::vector<T>& v, const T& arg, const Args&... args)
{
	fillVectorWithElements<T>(v, arg);
	fillVectorWithElements<T>(v, args...);
}

template<typename T, typename... Args>
std::vector<T> utility::createVectorFromElements(const Args&... args)
{
	std::vector<T> v;
	fillVectorWithElements<T>(v, args...);
	return v;
}

template<typename SourceType, typename TargetType>
std::vector<TargetType> utility::convert(const std::vector<SourceType>& sourceContainer, std::function<TargetType(const SourceType&)> conversion)
{
	std::vector<TargetType> targetContainer;
	for (const SourceType& sourceElement: sourceContainer)
	{
		targetContainer.push_back(conversion(sourceElement));
	}
	return targetContainer;
}

template<typename T>
std::vector<std::string> utility::toStrings(const std::vector<T>& d)
{
	return convert<T, std::string>(d, [](T t) { return std::to_string(t); });
}

template<>
inline std::vector<std::string> utility::toStrings<FilePath>(const std::vector<FilePath>& d)
{
	return convert<FilePath, std::string>(d, [](const FilePath& fp) { return utility::encodeToUtf8(fp.wstr()); });
}

template<typename T>
std::vector<std::wstring> utility::toWStrings(const std::vector<T>& d)
{
	return convert(d, [](T t) { return std::to_wstring(t); });
}

template<>
inline std::vector<std::wstring> utility::toWStrings<FilePath>(const std::vector<FilePath>& d)
{
	return convert<FilePath, std::wstring>(d, [](const FilePath& fp) { return fp.wstr(); });
}

#endif // UTILITY_H
