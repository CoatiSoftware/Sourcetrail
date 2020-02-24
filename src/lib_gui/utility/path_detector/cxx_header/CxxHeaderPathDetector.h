#ifndef CXX_HEADER_PATH_DETECTOR_H
#define CXX_HEADER_PATH_DETECTOR_H

#include "PathDetector.h"

class CxxHeaderPathDetector: public PathDetector
{
public:
	CxxHeaderPathDetector(const std::string& compilerName);

private:
	std::vector<FilePath> doGetPaths() const override;

	const std::string m_compilerName;
};

#endif	  // CXX_HEADER_PATH_DETECTOR_H
