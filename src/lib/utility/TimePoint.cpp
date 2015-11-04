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
	: m_time(boost::posix_time::not_a_date_time)
{
	if (s.size())
	{
		m_time = boost::posix_time::time_from_string(s);
	}
}

std::string TimePoint::toString() const
{
	std::stringstream stream;
	boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
	facet->format("%Y-%m-%d %H:%M:%S");
	stream.imbue(std::locale(std::locale::classic(), facet));
	stream << m_time;
	return stream.str();
}