#ifndef LOGGING_H
#define LOGGING_H

#include "utility/logging/LogManager.h"

/**
* @brief Makros to simplify usage of the log manager
*/
#define LOG_INFO(str) \
	do \
	{ \
		LogManager::getInstance()->logInfo(str, __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0) \

#define LOG_WARNING(str) \
	do \
	{ \
		LogManager::getInstance()->logWarning(str, __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0) \

#define LOG_ERROR(str) \
	do \
	{ \
		LogManager::getInstance()->logError(str, __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0) \

#endif // LOGGING_H
