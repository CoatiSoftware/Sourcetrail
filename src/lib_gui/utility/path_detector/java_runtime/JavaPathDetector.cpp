#include "utility/path_detector/java_runtime/JavaPathDetector.h"

JavaPathDetector::JavaPathDetector(const std::string& name, const std::string& javaVersion)
	: PathDetector(name)
	, m_javaVersion(javaVersion)
{
}

JavaPathDetector::~JavaPathDetector()
{
}
