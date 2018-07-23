#ifndef COMBINED_PATH_DETECTOR_H
#define COMBINED_PATH_DETECTOR_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "utility/path_detector/PathDetector.h"

class CombinedPathDetector: public PathDetector
{
public:
	CombinedPathDetector();

	void addDetector(std::shared_ptr<PathDetector> detector);

    std::vector<std::string> getWorkingDetectorNames();

	std::vector<FilePath> getPaths() const override;
	std::vector<FilePath> getPaths(std::string detectorName) const;

private:
	std::vector<std::shared_ptr<PathDetector>> m_detectors;
};

#endif // COMBINED_PATH_DETECTOR_H
