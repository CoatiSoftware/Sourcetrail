#include "PathDetector.h"

#include "FilePath.h"

PathDetector::PathDetector(const std::string& name): m_name(name) {}

std::string PathDetector::getName() const
{
	return m_name;
}

bool PathDetector::isWorking() const
{
	return (!getPaths().empty());
}
