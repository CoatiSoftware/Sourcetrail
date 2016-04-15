#ifndef COMPILER_DETECTOR_H
#define COMPILER_DETECTOR_H

#include "utility/headerSearch/DetectorBase.h"

class CompilerDetector
	: public DetectorBase
{
public:
	CompilerDetector(const std::string& name);

	virtual ~CompilerDetector();

	virtual std::vector<FilePath> getStandardHeaderPaths();
	virtual std::vector<FilePath> getStandardFrameworkPaths();

private:
	std::vector<std::string> getHeaderPaths();
};

#endif // COMPILER_DETECTOR_H
