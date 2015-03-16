#ifndef UTILITY_H
#define UTILITY_H

#include <chrono>

namespace utility
{
	float duration(std::function<void()> func)
	{
		std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

		func();

		std::chrono::duration<float> duration =
			std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start);
		return duration.count();
	}

	template<typename T>
	std::vector<T> concat(const std::vector<T>& a, const std::vector<T>& b)
	{
		std::vector<T> r(a.size() + b.size());
		append(r, a);
		append(r, b);
		return r;
	}

	template<typename T>
	void append(std::vector<T>& a, const std::vector<T>& b)
	{
		a.insert(a.begin(), b.begin(), b.end());
	}

	template<typename T>
	void append(std::set<T>& a, const std::set<T>& b)
	{
		a.insert(b.begin(), b.end());
	}
}

#endif // UTILITY_H
