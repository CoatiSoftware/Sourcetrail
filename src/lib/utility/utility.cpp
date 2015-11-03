#include "utility/utility.h"
#include "boost/date_time/time_clock.hpp"

TimePoint utility::durationStart()
{
	return TimePoint(boost::posix_time::microsec_clock::local_time());
}

float utility::duration(const TimePoint& start)
{
	TimePoint now = durationStart();
	return now - start;
}

float utility::duration(std::function<void()> func)
{
	const TimePoint start = durationStart();

	func();

	return duration(start);
}

std::string utility::timeToString(const time_t time)
{
	char buff[20];
	strftime(buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&time));
	return std::string(buff);
}

std::string utility::timeToString(const boost::posix_time::ptime time)
{
	return TimePoint(time).toString();
}

bool utility::intersectionPoint(Vec2f a1, Vec2f b1, Vec2f a2, Vec2f b2, Vec2f* i)
{
	Vec2f p = a1;
	Vec2f v = b1 - a1;
	Vec2f q = a2;
	Vec2f w = b2 - a2;

	float denominator = v.x * w.y - v.y * w.x;
	if (denominator != 0)
	{
		float t = (p.y * w.x - p.x * w.y + q.x * w.y - q.y * w.x) / denominator;
		float s = (q.y * v.x - q.x * v.y + p.x * v.y - p.y * v.x) / -denominator;

		if (t >= 0 && t <= 1 && s >= 0 && s <= 1)
		{
			*i = p + v * t;
			return true;
		}
	}

	return false;
}

size_t utility::digits(size_t n)
{
	int digits = 1;

	while (n >= 10)
	{
		n /= 10;
		digits++;
	}

	return digits;
}

int utility::roundToInt(float n)
{
	return (n > 0.0f) ? (n + 0.5f) : (n - 0.5f);
}
