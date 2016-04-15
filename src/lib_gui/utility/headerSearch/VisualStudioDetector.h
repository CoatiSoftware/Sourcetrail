#ifndef VISUAL_STUDIO_DETECTOR_H
#define VISUAL_STUDIO_DETECTOR_H

#include <vector>

#include "utility/headerSearch/DetectorBase.h"

class FilePath;

class VisualStudioDetector: public DetectorBase
{
public:
	VisualStudioDetector(const std::string name, int version, bool isExpress);
	virtual ~VisualStudioDetector();

	virtual std::vector<FilePath> getStandardHeaderPaths();
	virtual std::vector<FilePath> getStandardFrameworkPaths();

private:
	FilePath getVsInstallPathUsingRegistry();
	FilePath getWindowsSdkPathUsingRegistry(const std::string& version);

	int m_version;
	bool m_isExpress;
};

#endif // VISUAL_STUDIO_DETECTOR_H
