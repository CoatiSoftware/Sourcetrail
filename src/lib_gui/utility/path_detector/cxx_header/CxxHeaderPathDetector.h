#ifndef CXX_HEADER_PATH_DETECTOR_H
#define CXX_HEADER_PATH_DETECTOR_H

#include "PathDetector.h"

class CxxHeaderPathDetector: public PathDetector
{
public:
	CxxHeaderPathDetector(const std::string& compilerName);
	std::vector<FilePath> getPaths() const override;

private:
	const std::string m_compilerName;
};

#endif // CXX_HEADER_PATH_DETECTOR_H
