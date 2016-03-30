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
	virtual std::string getName() const;
	virtual void setName(const std::string& name);
protected:
	std::string m_name;
};

class Detector
{
public:
	virtual ~Detector();

};


#endif // DETECTOR_BASE_H

