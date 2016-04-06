#ifndef VISUAL_STUDIO_DETECTOR_H
#define VISUAL_STUDIO_DETECTOR_H

#include <vector>

#include "utility/headerSearch/DetectorBase.h"

class FilePath;

class VisualStudioDetector : public DetectorBase
{
public:
	VisualStudioDetector(const std::string name = "14");
	virtual ~VisualStudioDetector();
	virtual std::vector<FilePath> getStandardHeaderPaths();
private:
	std::string getFullName();
	bool getStanardHeaderPathsUsingEnvironmentVariable(std::vector<FilePath>& paths);
	bool getStanardHeaderPathsUsingRegistry(std::vector<FilePath>& paths, bool lookForExpressVersion = false);

	void setName(const std::string& version);
	int m_versionNumber;
	bool m_isExpress;
};

#endif // VISUAL_STUDIO_DETECTOR_H
