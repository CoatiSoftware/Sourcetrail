#include "utility/utility.h"

#include <iomanip>
#include <sstream>

#include "boost/date_time/time_clock.hpp"


ApplicationArchitectureType utility::getApplicationArchitectureType()
{
#if defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64) || defined(WIN64)
    return APPLICATION_ARCHITECTURE_X86_64;
#else
    return APPLICATION_ARCHITECTURE_X86_32;
#endif
    return APPLICATION_ARCHITECTURE_UNKNOWN;
}

TimeStamp utility::durationStart()
{
	return TimeStamp::now();
}

float utility::duration(const TimeStamp& start)
{
	TimeStamp now = durationStart();
	return float(now.deltaMS(start)) / 1000.0f;
}

float utility::duration(std::function<void()> func)
{
	const TimeStamp start = durationStart();

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
	return TimeStamp(time).toString();
}

std::string utility::timeToString(float secondsTotal)
{
	std::stringstream ss;

	int hours = int(secondsTotal / 3600);
	secondsTotal -= hours * 3600;
	int minutes = int(secondsTotal / 60);
	secondsTotal -= minutes * 60;
	int seconds = int(secondsTotal);
	secondsTotal -= seconds;
	int milliSeconds = secondsTotal * 1000;

	if (hours > 9)
	{
		ss << hours;
	}
	else
	{
		ss << std::setw(2) << std::setfill('0') << hours;
	}
	ss << ":" << std::setw(2) << std::setfill('0') << minutes;
	ss << ":" << std::setw(2) << std::setfill('0') << seconds;

	if (!hours && !minutes)
	{
		ss << ":" << std::setw(3) << std::setfill('0') << milliSeconds;
	}

	return ss.str();
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
