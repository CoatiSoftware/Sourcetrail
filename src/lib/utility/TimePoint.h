#ifndef TIME_POINT_H
#define TIME_POINT_H

#include "boost/date_time/posix_time/posix_time.hpp"

class TimePoint
{
public:
	static TimePoint now();

	TimePoint();
	TimePoint(boost::posix_time::ptime t);
	//TimePoint(time_t t);
	TimePoint(std::string s);

	bool isValid() const;

	std::string toString() const;

	inline bool operator==(const TimePoint& rhs){ return m_time == rhs.m_time; }
	inline bool operator!=(const TimePoint& rhs){ return m_time != rhs.m_time; }
	inline bool operator<(const TimePoint& rhs){ return m_time < rhs.m_time; }
	inline bool operator>(const TimePoint& rhs){ return m_time > rhs.m_time; }
	inline bool operator<=(const TimePoint& rhs){ return m_time <= rhs.m_time; }
	inline bool operator>=(const TimePoint& rhs){ return m_time >= rhs.m_time; }

	inline float operator-(const TimePoint& rhs){ return deltaMS(rhs) / 1000.0f; }

	size_t deltaMS(const TimePoint& other) const;

private:
	boost::posix_time::ptime m_time;
};

#endif // TIME_POINT_H
