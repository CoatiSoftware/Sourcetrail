#ifndef JAVA_PATH_DETECTOR_MAC_H
#define JAVA_PATH_DETECTOR_MAC_H

#include "JavaPathDetector.h"

class JavaPathDetectorMac
	: public JavaPathDetector
{
public:
	JavaPathDetectorMac(const std::string javaVersion);
	virtual std::vector<FilePath> getPaths() const override;
};

#endif // JAVA_PATH_DETECTOR_MAC_H
