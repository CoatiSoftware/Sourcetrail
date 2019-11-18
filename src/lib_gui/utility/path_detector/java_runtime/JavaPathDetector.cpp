#include "JavaPathDetector.h"

JavaPathDetector::JavaPathDetector(const std::string& name, const std::string& javaVersion)
	: PathDetector(name), m_javaVersion(javaVersion)
{
}
