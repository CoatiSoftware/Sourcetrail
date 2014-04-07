#include "utility/logging/Logger.h"

Logger::Logger(const std::string& type)
	: m_type(type)
{
}

Logger::~Logger()
{
}

std::string Logger::getType() const
{
	return m_type;
}
