#ifndef DETECTOR_BASE_H
#define DETECTOR_BASE_H

#include <vector>

#include "utility/file/FilePath.h"

class DetectorBase
{
public:
	DetectorBase(const std::string name);
	virtual ~DetectorBase();

	std::string getName() const;
	bool isWorking();

	virtual std::vector<FilePath> getStandardHeaderPaths() = 0;
	virtual std::vector<FilePath> getStandardFrameworkPaths() = 0;

protected:
	std::string m_name;
};

#endif // DETECTOR_BASE_H
