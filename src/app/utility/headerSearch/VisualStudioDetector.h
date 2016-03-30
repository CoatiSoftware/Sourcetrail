#ifndef VISUAL_STUDIO_DETECTOR_H
#define VISUAL_STUDIO_DETECTOR_H

#include <vector>
#include "utility/headerSearch/DetectorBase.h"

class FilePath;

class VisualStudioDetector : public DetectorBase
{
public:
	VisualStudioDetector(const std::string name = "VS140");
	virtual ~VisualStudioDetector();
	virtual std::vector<FilePath> getStandardHeaderPaths();
private:
	std::string getFullName();

};

#endif // VISUAL_STUDIO_DETECTOR_H
