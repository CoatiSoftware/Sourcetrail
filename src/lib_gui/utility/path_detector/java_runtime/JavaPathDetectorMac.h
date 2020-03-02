#ifndef JAVA_PATH_DETECTOR_MAC_H
#define JAVA_PATH_DETECTOR_MAC_H

#include "JavaPathDetector.h"

class JavaPathDetectorMac: public JavaPathDetector
{
public:
	JavaPathDetectorMac(const std::string javaVersion);

private:
	virtual std::vector<FilePath> doGetPaths() const override;
};

#endif	  // JAVA_PATH_DETECTOR_MAC_H
