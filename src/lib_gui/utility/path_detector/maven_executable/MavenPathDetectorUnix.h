#ifndef MAVEN_PATH_DETECTOR_UNIX_H
#define MAVEN_PATH_DETECTOR_UNIX_H

#include "PathDetector.h"

class MavenPathDetectorUnix: public PathDetector
{
public:
	MavenPathDetectorUnix();
	std::vector<FilePath> getPaths() const override;
};

#endif // MAVEN_PATH_DETECTOR_UNIX_H
