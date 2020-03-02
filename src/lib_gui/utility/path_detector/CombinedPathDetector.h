#ifndef COMBINED_PATH_DETECTOR_H
#define COMBINED_PATH_DETECTOR_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include "PathDetector.h"

class CombinedPathDetector: public PathDetector
{
public:
	CombinedPathDetector();

	void addDetector(std::shared_ptr<PathDetector> detector);

	std::vector<std::string> getWorkingDetectorNames();

	std::vector<FilePath> getPathsForDetector(const std::string& detectorName) const;

private:
	std::vector<FilePath> doGetPaths() const override;

	std::vector<std::shared_ptr<PathDetector>> m_detectors;
};

#endif	  // COMBINED_PATH_DETECTOR_H
