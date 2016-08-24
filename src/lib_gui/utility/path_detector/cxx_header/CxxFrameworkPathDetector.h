#ifndef CXX_FRAMEWORK_PATH_DETECTOR_H
#define CXX_FRAMEWORK_PATH_DETECTOR_H

#include "utility/path_detector/PathDetector.h"

class CxxFrameworkPathDetector: public PathDetector
{
public:
	CxxFrameworkPathDetector(const std::string& compilerName);
	virtual ~CxxFrameworkPathDetector();

	virtual std::vector<FilePath> getPaths() const;

private:
	const std::string m_compilerName;
};

#endif // CXX_FRAMEWORK_PATH_DETECTOR_H
