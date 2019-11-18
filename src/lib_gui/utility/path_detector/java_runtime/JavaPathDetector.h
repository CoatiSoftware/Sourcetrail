#ifndef JAVA_PATH_DETECTOR_H
#define JAVA_PATH_DETECTOR_H

#include "PathDetector.h"

class JavaPathDetector: public PathDetector
{
public:
	JavaPathDetector(const std::string& name, const std::string& javaVersion);

protected:
	const std::string m_javaVersion;
};

#endif	  // JAVA_PATH_DETECTOR_H
