#ifndef COMPILER_DETECTOR_BASE_H
#define COMPILER_DETECTOR_BASE_H

#include "utility/headerSearch/DetectorBase.h"

class CompilerDetectorBase : public DetectorBase
{
public:
	virtual std::vector<FilePath> getStandardHeaderPaths();
private:
};

#endif // COMPILER_DETECTOR_BASE_H
