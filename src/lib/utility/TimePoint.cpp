#include "TimePoint.h"

TimePoint::TimePoint()
	: m_time(boost::posix_time::not_a_date_time)
{
}

TimePoint::TimePoint(boost::posix_time::ptime t)
	: m_time(t)
{
}

//TimePoint::TimePoint(time_t t)
//{
// needs an implementation
//}

TimePoint::TimePoint(std::string s)
{
	m_time = boost::posix_time::time_from_string(s);
}
