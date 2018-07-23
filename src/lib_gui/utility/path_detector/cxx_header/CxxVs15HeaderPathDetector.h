#ifndef CXX_VS_15_HEADER_PATH_DETECTOR_H
#define CXX_VS_15_HEADER_PATH_DETECTOR_H

#include "utility/path_detector/PathDetector.h"
#include "utility/ApplicationArchitectureType.h"

class CxxVs15HeaderPathDetector: public PathDetector
{
public:
	CxxVs15HeaderPathDetector();
	std::vector<FilePath> getPaths() const override;
};

#endif // CXX_VS_15_HEADER_PATH_DETECTOR_H
