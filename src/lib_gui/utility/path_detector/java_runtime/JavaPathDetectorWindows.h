#ifndef JAVA_PATH_DETECTOR_WINDOWS_H
#define JAVA_PATH_DETECTOR_WINDOWS_H

#include "JavaPathDetector.h"

class JavaPathDetectorWindows: public JavaPathDetector
{
public:
	JavaPathDetectorWindows(const std::string javaVersion);

private:
	virtual std::vector<FilePath> doGetPaths() const override;
};

#endif	  // JAVA_PATH_DETECTOR_WINDOWS_H
