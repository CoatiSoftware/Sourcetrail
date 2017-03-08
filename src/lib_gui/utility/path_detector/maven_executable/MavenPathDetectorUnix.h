#ifndef MAVEN_PATH_DETECTOR_UNIX_H
#define MAVEN_PATH_DETECTOR_UNIX_H

#include "utility/path_detector/PathDetector.h"

class MavenPathDetectorUnix: public PathDetector
{
public:
	MavenPathDetectorUnix();
	virtual ~MavenPathDetectorUnix();

	virtual std::vector<FilePath> getPaths() const;
};

#endif // MAVEN_PATH_DETECTOR_UNIX_H
