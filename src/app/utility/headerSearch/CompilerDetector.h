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

static StandardHeaderDetection::Add<CompilerDetector> gcc("gcc");
static StandardHeaderDetection::Add<CompilerDetector> clang("clang");
static StandardHeaderDetection::Add<CompilerDetector> fakecomplier("fakecompiler");

#endif // COMPILER_DETECTOR_H
