#ifndef TIME_POINT_H
#define TIME_POINT_H

#include "boost/date_time/posix_time/posix_time.hpp"

class TimePoint // that name sounds pretty silly, was time stamp not ok?
{
public:
	static TimePoint now();

	TimePoint();
	TimePoint(boost::posix_time::ptime t);
	//TimePoint(time_t t);
	TimePoint(std::string s);

	bool isValid() const;

	std::string toString() const;
	std::string getDDMMYYYYString() const;

	inline bool operator==(const TimePoint& rhs){ return m_time == rhs.m_time; }
	inline bool operator!=(const TimePoint& rhs){ return m_time != rhs.m_time; }
	inline bool operator<(const TimePoint& rhs){ return m_time < rhs.m_time; }
	inline bool operator>(const TimePoint& rhs){ return m_time > rhs.m_time; }
	inline bool operator<=(const TimePoint& rhs){ return m_time <= rhs.m_time; }
	inline bool operator>=(const TimePoint& rhs){ return m_time >= rhs.m_time; }

	inline float operator-(const TimePoint& rhs){ return deltaMS(rhs) / 1000.0f; }

	size_t deltaMS(const TimePoint& other) const;

	bool isSameDay(const TimePoint& other) const;
	size_t deltaDays(const TimePoint& other) const; // days are counted beginning at 00:00, so a tp of 1.1.2017 23:59 is 1 day ago if it's the 2.1.2017 00:01

private:
	boost::posix_time::ptime m_time;
};

#endif // TIME_POINT_H
