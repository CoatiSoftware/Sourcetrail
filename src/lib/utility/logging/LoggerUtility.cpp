#include "LoggerUtility.h"

#include <sstream>
#include <ctime>

std::string LoggerUtility::generateDatedFileName(const std::string& prefix, const std::string& suffix)
{
	time_t time;
	std::time(&time);
	tm t = *std::localtime(&time);

	std::stringstream filename;
	if (prefix.length() > 0)
	{
		filename << prefix;
		filename << "_";
	}
	filename << t.tm_year + 1900 << "-";
	filename << (t.tm_mon < 9 ? "0" : "") << t.tm_mon + 1 << "-";
	filename << (t.tm_mday < 10 ? "0" : "") << t.tm_mday << "_";
	filename << (t.tm_hour < 10 ? "0" : "") << t.tm_hour << "-";
	filename << (t.tm_min < 10 ? "0" : "") << t.tm_min << "-";
	filename << (t.tm_sec < 10 ? "0" : "") << t.tm_sec;

	if (suffix.length() > 0)
	{
		filename << "_";
		filename << suffix;
	}

	return filename.str();
}
