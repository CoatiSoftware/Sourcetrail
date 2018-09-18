#ifndef LOGGING_H
#define LOGGING_H

#include <sstream>

#include "LogManager.h"

/**
* @brief Makros to simplify usage of the log manager
*/
#define LOG_INFO(__str__) \
	do \
	{ \
		LogManager::getInstance()->logInfo(__str__, __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0)

#define LOG_WARNING(__str__) \
	do \
	{ \
		LogManager::getInstance()->logWarning(__str__, __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0)

#define LOG_ERROR(__str__) \
	do \
	{ \
		LogManager::getInstance()->logError(__str__, __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0)

#define LOG_INFO_BARE(__str__) \
	do \
	{ \
		LogManager::getInstance()->logInfo(__str__, "", "", 0); \
	} \
	while(0)

#define LOG_WARNING_BARE(__str__) \
	do \
	{ \
		LogManager::getInstance()->logWarning(__str__, "", "", 0); \
	} \
	while(0)

#define LOG_ERROR_BARE(__str__) \
	do \
	{ \
		LogManager::getInstance()->logError(__str__, "", "", 0); \
	} \
	while(0)

#define LOG_INFO_STREAM(__s__) \
	do \
	{ \
		std::stringstream __ss__; \
		__ss__ __s__; \
		LogManager::getInstance()->logInfo(__ss__.str(), __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0)

#define LOG_WARNING_STREAM(__s__) \
	do \
	{ \
		std::stringstream __ss__; \
		__ss__ __s__; \
		LogManager::getInstance()->logWarning(__ss__.str(), __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0)

#define LOG_ERROR_STREAM(__s__) \
	do \
	{ \
		std::stringstream __ss__; \
		__ss__ __s__; \
		LogManager::getInstance()->logError(__ss__.str(), __FILE__, __FUNCTION__, __LINE__); \
	} \
	while(0)

#define LOG_INFO_STREAM_BARE(__s__) \
	do \
	{ \
		std::stringstream __ss__; \
		__ss__ __s__; \
		LogManager::getInstance()->logInfo(__ss__.str(), "", "", 0); \
	} \
	while(0)

#define LOG_WARNING_STREAM_BARE(__s__) \
	do \
	{ \
		std::stringstream __ss__; \
		__ss__ __s__; \
		LogManager::getInstance()->logWarning(__ss__.str(), "", "", 0); \
	} \
	while(0)

#define LOG_ERROR_STREAM_BARE(__s__) \
	do \
	{ \
		std::stringstream __ss__; \
		__ss__ __s__; \
		LogManager::getInstance()->logError(__ss__.str(), "", "", 0); \
	} \
	while(0)

#endif // LOGGING_H
