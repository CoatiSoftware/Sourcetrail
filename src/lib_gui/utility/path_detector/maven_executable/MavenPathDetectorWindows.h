#ifndef MAVEN_PATH_DETECTOR_WINDOWS_H
#define MAVEN_PATH_DETECTOR_WINDOWS_H

#include "PathDetector.h"

class MavenPathDetectorWindows: public PathDetector
{
public:
	MavenPathDetectorWindows();
	std::vector<FilePath> getPaths() const override;
};

#endif	  // MAVEN_PATH_DETECTOR_WINDOWS_H
