#ifndef MAVEN_PATH_DETECTOR_WINDOWS_H
#define MAVEN_PATH_DETECTOR_WINDOWS_H

#include "PathDetector.h"

class MavenPathDetectorWindows: public PathDetector
{
public:
	MavenPathDetectorWindows();

private:
	std::vector<FilePath> doGetPaths() const override;
};

#endif	  // MAVEN_PATH_DETECTOR_WINDOWS_H
