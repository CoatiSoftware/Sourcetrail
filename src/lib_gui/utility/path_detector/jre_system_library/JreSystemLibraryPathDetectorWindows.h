#ifndef JRE_SYSTEM_LIBRARY_PATH_DETECTOR_WINDOWS_H
#define JRE_SYSTEM_LIBRARY_PATH_DETECTOR_WINDOWS_H

#include "utility/path_detector/jre_system_library/JreSystemLibraryPathDetector.h"

class JreSystemLibraryPathDetectorWindows: public JreSystemLibraryPathDetector
{
public:
	JreSystemLibraryPathDetectorWindows(const std::string javaVersion);
	virtual ~JreSystemLibraryPathDetectorWindows();
};

#endif // JRE_SYSTEM_LIBRARY_PATH_DETECTOR_WINDOWS_H
