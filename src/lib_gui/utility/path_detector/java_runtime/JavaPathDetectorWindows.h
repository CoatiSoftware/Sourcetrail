#ifndef JAVA_PATH_DETECTOR_WINDOWS_H
#define JAVA_PATH_DETECTOR_WINDOWS_H

#include "utility/path_detector/java_runtime/JavaPathDetector.h"

class JavaPathDetectorWindows: public JavaPathDetector
{
public:
	JavaPathDetectorWindows(const std::string javaVersion);
	virtual std::vector<FilePath> getPaths() const override;
};

#endif // JAVA_PATH_DETECTOR_WINDOWS_H
