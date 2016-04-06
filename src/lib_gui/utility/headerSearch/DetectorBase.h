#ifndef DETECTOR_BASE_H
#define DETECTOR_BASE_H

#include <vector>

#include "utility/file/FilePath.h"

class DetectorBase
{
public:
	DetectorBase(const std::string name);
	virtual ~DetectorBase(){};
	virtual bool detect();
	virtual std::vector<FilePath> getStandardHeaderPaths() = 0;
	virtual std::vector<FilePath> getStandardFrameworkPaths();
	virtual std::string getName() const;
	virtual void setName(const std::string& name);
protected:
	std::string m_name;
};

#endif // DETECTOR_BASE_H
