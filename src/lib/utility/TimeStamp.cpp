#include "TimeStamp.h"

TimeStamp TimeStamp::now()
{
	return TimeStamp(boost::posix_time::microsec_clock::local_time());
}

TimeStamp::TimeStamp()
	: m_time(boost::posix_time::not_a_date_time)
{
}

TimeStamp::TimeStamp(boost::posix_time::ptime t)
	: m_time(t)
{
}

TimeStamp::TimeStamp(std::string s)
	: m_time(boost::posix_time::not_a_date_time)
{
	if (s.size())
	{
		m_time = boost::posix_time::time_from_string(s);
	}
}

bool TimeStamp::isValid() const
{
	return m_time != boost::posix_time::not_a_date_time;
}

std::string TimeStamp::toString() const
{
	std::stringstream stream;
	boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
	facet->format("%Y-%m-%d %H:%M:%S");
	stream.imbue(std::locale(std::locale::classic(), facet));
	stream << m_time;
	return stream.str();
}

std::string TimeStamp::getDDMMYYYYString() const
{
	std::stringstream stream;
	boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
	facet->format("%d-%m-%Y");
	stream.imbue(std::locale(std::locale::classic(), facet));
	stream << m_time;
	return stream.str();
}

size_t TimeStamp::deltaMS(const TimeStamp& other) const
{
	return abs((m_time - other.m_time).total_milliseconds());
}

size_t TimeStamp::deltaS(const TimeStamp& other) const
{
	return abs((m_time - other.m_time).total_seconds());
}

bool TimeStamp::isSameDay(const TimeStamp& other) const
{
	if (m_time.date().day() == other.m_time.date().day() &&
		m_time.date().month() == other.m_time.date().month() &&
		m_time.date().year() == other.m_time.date().year())
	{
		return true;
	}

	return false;
}

size_t TimeStamp::deltaDays(const TimeStamp& other) const
{
	boost::gregorian::date_duration deltaDate = m_time.date() - other.m_time.date();
	return abs(deltaDate.days());
}

size_t TimeStamp::deltaHours(const TimeStamp& other) const
{
	boost::posix_time::time_duration delta = m_time - other.m_time;
	return abs(delta.total_seconds() / 3600);
}
