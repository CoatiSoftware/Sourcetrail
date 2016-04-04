#ifndef COMPILER_DETECTOR_H
#define COMPILER_DETECTOR_H

#include "utility/headerSearch/DetectorBase.h"
#include "utility/headerSearch/StandardHeaderDetection.h"

class CompilerDetector : public DetectorBase
{
public:
	CompilerDetector(const std::string& name);
	virtual ~CompilerDetector();
	virtual std::vector<FilePath> getStandardHeaderPaths();
private:
};


#endif // COMPILER_DETECTOR_H
