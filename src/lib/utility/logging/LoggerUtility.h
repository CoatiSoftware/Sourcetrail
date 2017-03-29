#ifndef LOGGER_UTILITY_H
#define LOGGER_UTILITY_H

#include <string>

class LoggerUtility
{
public:
	static std::string generateDatedFileName(const std::string& prefix = "", const std::string& suffix = "");
};

#endif // LOGGER_UTILITY_H