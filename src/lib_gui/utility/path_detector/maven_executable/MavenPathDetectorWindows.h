#ifndef MAVEN_PATH_DETECTOR_WINDOWS_H
#define MAVEN_PATH_DETECTOR_WINDOWS_H

#include "utility/path_detector/PathDetector.h"

class MavenPathDetectorWindows: public PathDetector
{
public:
	MavenPathDetectorWindows();
	virtual ~MavenPathDetectorWindows();

	virtual std::vector<FilePath> getPaths() const;
};

#endif // MAVEN_PATH_DETECTOR_WINDOWS_H
