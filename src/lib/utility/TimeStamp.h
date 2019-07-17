#ifndef TIME_STAMP_H
#define TIME_STAMP_H

#include <boost/date_time/posix_time/posix_time.hpp>

class TimeStamp
{
public:
	static TimeStamp now();

	static double durationSeconds(const TimeStamp& start);
	static std::string secondsToString(double seconds);

	TimeStamp();
	TimeStamp(boost::posix_time::ptime t);
	TimeStamp(std::string s);

	bool isValid() const;

	std::string toString() const;
	std::string getDDMMYYYYString() const;
	std::string dayOfWeek() const;
	std::string dayOfWeekShort() const;

	inline bool operator==(const TimeStamp& rhs) { return m_time == rhs.m_time; }
	inline bool operator!=(const TimeStamp& rhs) { return m_time != rhs.m_time; }
	inline bool operator<(const TimeStamp& rhs) { return m_time < rhs.m_time; }
	inline bool operator>(const TimeStamp& rhs) { return m_time > rhs.m_time; }
	inline bool operator<=(const TimeStamp& rhs) { return m_time <= rhs.m_time; }
	inline bool operator>=(const TimeStamp& rhs) { return m_time >= rhs.m_time; }

	size_t deltaMS(const TimeStamp& other) const;
	size_t deltaS(const TimeStamp& other) const;

	bool isSameDay(const TimeStamp& other) const;

	// days are counted beginning at 00:00, so a tp of 1.1.2017 23:59 is 1 day ago if it's the 2.1.2017 00:01
	size_t deltaDays(const TimeStamp& other) const;
	size_t deltaHours(const TimeStamp& other) const;

private:
	boost::posix_time::ptime m_time;
};

#endif // TIME_STAMP_H
