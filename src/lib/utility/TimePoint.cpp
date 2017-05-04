#include "TimePoint.h"

TimePoint TimePoint::now()
{
	return TimePoint(boost::posix_time::microsec_clock::local_time());
}

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

bool TimePoint::isValid() const
{
	return m_time != boost::posix_time::not_a_date_time;
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

std::string TimePoint::getDDMMYYYYString() const
{
	std::stringstream stream;
	boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
	facet->format("%d-%m-%Y");
	stream.imbue(std::locale(std::locale::classic(), facet));
	stream << m_time;
	return stream.str();
}

size_t TimePoint::deltaMS(const TimePoint& other) const
{
	return (m_time - other.m_time).total_milliseconds();
}

size_t TimePoint::deltaS(const TimePoint& other) const
{
	return (m_time - other.m_time).total_seconds();
}

bool TimePoint::isSameDay(const TimePoint& other) const
{
	if (m_time.date().day() == other.m_time.date().day() &&
		m_time.date().month() == other.m_time.date().month() &&
		m_time.date().year() == other.m_time.date().year())
	{
		return true;
	}

	return false;
}

size_t TimePoint::deltaDays(const TimePoint& other) const
{
	boost::gregorian::date_duration deltaDate = m_time.date() - other.m_time.date();
	return size_t(std::abs(deltaDate.days()));
}
