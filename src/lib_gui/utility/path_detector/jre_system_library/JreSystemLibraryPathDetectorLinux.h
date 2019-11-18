#ifndef JRE_SYSTEM_LIBRARY_PATH_DETECTOR_LINUX_H
#define JRE_SYSTEM_LIBRARY_PATH_DETECTOR_LINUX_H

#include "JreSystemLibraryPathDetector.h"

class JreSystemLibraryPathDetectorLinux: public JreSystemLibraryPathDetector
{
public:
	JreSystemLibraryPathDetectorLinux(const std::string javaVersion);
};

#endif	  // JRE_SYSTEM_LIBRARY_PATH_DETECTOR_LINUX_H
