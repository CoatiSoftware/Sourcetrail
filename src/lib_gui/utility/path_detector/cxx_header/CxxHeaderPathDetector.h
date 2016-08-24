#ifndef CXX_HEADER_PATH_DETECTOR_H
#define CXX_HEADER_PATH_DETECTOR_H

#include "utility/path_detector/PathDetector.h"

class CxxHeaderPathDetector: public PathDetector
{
public:
	CxxHeaderPathDetector(const std::string& compilerName);
	virtual ~CxxHeaderPathDetector();

	virtual std::vector<FilePath> getPaths() const;

private:
	const std::string m_compilerName;
};

#endif // CXX_HEADER_PATH_DETECTOR_H
