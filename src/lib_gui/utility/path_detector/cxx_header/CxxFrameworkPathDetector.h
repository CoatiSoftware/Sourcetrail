#ifndef CXX_FRAMEWORK_PATH_DETECTOR_H
#define CXX_FRAMEWORK_PATH_DETECTOR_H

#include "PathDetector.h"

class CxxFrameworkPathDetector: public PathDetector
{
public:
	CxxFrameworkPathDetector(const std::string& compilerName);
	std::vector<FilePath> getPaths() const override;

private:
	const std::string m_compilerName;
};

#endif // CXX_FRAMEWORK_PATH_DETECTOR_H
