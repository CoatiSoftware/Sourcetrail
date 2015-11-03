#ifndef TIME_POINT_H
#define TIME_POINT_H

#include "boost/date_time/posix_time/posix_time.hpp"

class TimePoint
{
public:
	TimePoint();
	TimePoint(boost::posix_time::ptime t);
	//TimePoint(time_t t);
	TimePoint(std::string s);

	inline bool operator==(const TimePoint& rhs){ return m_time == rhs.m_time; }
	inline bool operator!=(const TimePoint& rhs){ return m_time != rhs.m_time; }
	inline bool operator<(const TimePoint& rhs){ return m_time < rhs.m_time; }
	inline bool operator>(const TimePoint& rhs){ return m_time > rhs.m_time; }
	inline bool operator<=(const TimePoint& rhs){ return m_time <= rhs.m_time; }
	inline bool operator>=(const TimePoint& rhs){ return m_time >= rhs.m_time; }

private:
	boost::posix_time::ptime m_time;
};

#endif // TIME_POINT_H
