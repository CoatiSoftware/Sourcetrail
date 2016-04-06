#ifndef COMPILER_DETECTOR_H
#define COMPILER_DETECTOR_H

#include "utility/headerSearch/DetectorBase.h"

class CompilerDetector
	: public DetectorBase
{
public:
	CompilerDetector(const std::string& name);

	virtual ~CompilerDetector();

	std::vector<std::string> getHeaderPaths();

	virtual std::vector<FilePath> getStandardHeaderPaths();
	virtual std::vector<FilePath> getStandardFrameworkPaths();
};

#endif // COMPILER_DETECTOR_H
