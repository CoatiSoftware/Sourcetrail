#ifndef JRE_SYSTEM_LIBRARY_PATH_DETECTOR_WINDOWS_H
#define JRE_SYSTEM_LIBRARY_PATH_DETECTOR_WINDOWS_H

#include "JreSystemLibraryPathDetector.h"

class JreSystemLibraryPathDetectorWindows: public JreSystemLibraryPathDetector
{
public:
	JreSystemLibraryPathDetectorWindows(const std::string javaVersion);
};

#endif	  // JRE_SYSTEM_LIBRARY_PATH_DETECTOR_WINDOWS_H
