#ifndef CXX_FRAMEWORK_PATH_DETECTOR_H
#define CXX_FRAMEWORK_PATH_DETECTOR_H

#include "PathDetector.h"

class CxxFrameworkPathDetector: public PathDetector
{
public:
	CxxFrameworkPathDetector(const std::string& compilerName);

private:
	std::vector<FilePath> doGetPaths() const override;

	const std::string m_compilerName;
};

#endif	  // CXX_FRAMEWORK_PATH_DETECTOR_H
