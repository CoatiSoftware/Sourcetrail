#ifndef JAVA_PATH_DETECTOR_H
#define JAVA_PATH_DETECTOR_H

#include "utility/path_detector/PathDetector.h"
#include "utility/file/FilePath.h"

class JavaPathDetector: public PathDetector
{
public:
	JavaPathDetector(const std::string& name, const std::string& javaVersion);
	virtual ~JavaPathDetector();

protected:
	const std::string m_javaVersion;
};

#endif // JAVA_PATH_DETECTOR_H
