#include "utility/path_detector/PathDetector.h"

#include "utility/file/FilePath.h"

PathDetector::PathDetector(const std::string& name)
	: m_name(name)
{
}

PathDetector::~PathDetector()
{
}

std::string PathDetector::getName() const
{
	return m_name;
}

bool PathDetector::isWorking() const
{
	return (!getPaths().empty());
}
