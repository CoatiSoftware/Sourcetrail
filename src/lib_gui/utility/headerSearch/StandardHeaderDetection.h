#ifndef STANDARD_HEADER_DETECTION_H
#define STANDARD_HEADER_DETECTION_H

#include <map>
#include <memory>
#include <string>
#include <iostream>

#include "utility/headerSearch/DetectorBase.h"

class FilePath;

class StandardHeaderDetection
{
public:
    StandardHeaderDetection();
    ~StandardHeaderDetection();

	void addDetector(std::shared_ptr<DetectorBase> detector);

	/// Checks all availabe detectors and saves found Compilers
    void detectHeaders();

	/// Returns alls found compilers
    std::vector<std::string> getDetectedCompilers();

	/// Returns the headerpaths from a found compiler
    std::vector<FilePath> getStandardHeaderPaths(std::string compiler);
    std::vector<FilePath> getStandardFrameworkPaths(std::string compiler);

	/// Debugging Output
	void printAvailableDetectors();
	void printDetectedCompilers();

private:
	typedef std::map<std::string, std::shared_ptr<DetectorBase>> DetectorMap;
	typedef std::pair<std::string, std::shared_ptr<DetectorBase>> DetectorPair;

	static DetectorMap s_availableDetectors;
	static DetectorMap s_detectedCompilers;
};

#endif // STANDARD_HEADER_DETECTION_H
