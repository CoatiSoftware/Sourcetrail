#ifndef LOGGING_H
#define LOGGING_H

#include "utility/logging/LogManager.h"

/**
* @brief Makros to simplify usage of the log manager
*/
#define LOG_INFO(str) \
	do \
	{ \
		std::string s((str)); \
		LogManager::getInstance()->logInfo(s, __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0) \

#define LOG_WARNING(str) \
	do \
	{ \
		std::string s((str)); \
		LogManager::getInstance()->logWarning(s, __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0) \

#define LOG_ERROR(str) \
	do \
	{ \
		std::string s((str)); \
		LogManager::getInstance()->logError(s, __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0) \

#endif // LOGGING_H
