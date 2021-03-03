#ifndef JAVA_PATH_DETECTOR_WINDOWS_H
#define JAVA_PATH_DETECTOR_WINDOWS_H

#include "JavaPathDetector.h"

class JavaPathDetectorWindows: public JavaPathDetector
{
public:
	JavaPathDetectorWindows(const std::string javaVersion, bool isJre);

private:
	virtual std::vector<FilePath> doGetPaths() const override;
	bool m_isJre;
};

#endif	  // JAVA_PATH_DETECTOR_WINDOWS_H
