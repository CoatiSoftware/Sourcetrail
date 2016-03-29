#ifndef VISUAL_STUDIO_DETECTOR_H
#define VISUAL_STUDIO_DETECTOR_H

#include <vector>
#include "utility/headerSearch/DetectorBase.h"

class FilePath;

class VisualStudioDetector : public DetectorBase
{
public:
	virtual ~VisualStudioDetector();
	virtual std::vector<FilePath> getStandardHeaderPaths();
};

#endif // VISUAL_STUDIO_DETECTOR_H
