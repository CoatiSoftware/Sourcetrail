#ifndef STANDARDHEADER_DETECTION_H
#define STANDARDHEADER_DETECTION_H

#include <map>
#include <memory>
#include <string>

#include "utility/headerSearch/DetectorBase.h"

typedef std::map<std::string, std::shared_ptr<DetectorBase>> DetectorMap;

class FilePath;

class StandardHeaderDetection
{
public:
    StandardHeaderDetection();
    ~StandardHeaderDetection();

	/// Checks all availabe detectors and saves found Compilers
    void detectHeaders();

	/// Returns alls found compilers
    std::vector<std::string> getDetectedCompilers();

	/// Returns the headerpaths from a found compiler
    std::vector<FilePath> getStandardHeaderPaths(std::string compiler);

	/// Declare a instance of this class to add a detector
	template <typename U>
	class Add
	{
	public:
		Add(const std::string& name)
		{
			std::shared_ptr<DetectorBase> detector = std::shared_ptr<U>(new U());
			if (!name.empty())
			{
				detector->setName(name);
			}
			if (!s_availableDetectors)
			{
				s_availableDetectors = new DetectorMap();
			}
			s_availableDetectors->insert(std::make_pair(detector->getName(), detector));
		}
	};

	void printAvailableDetectors();
	void printdetectedCompilers();
private:
	static DetectorMap*  s_availableDetectors;
	DetectorMap m_detectedCompilers;
};

#endif //STANDARDHEADER_DETECTION_H
