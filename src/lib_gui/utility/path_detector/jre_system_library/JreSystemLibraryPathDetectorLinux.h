#ifndef JRE_SYSTEM_LIBRARY_PATH_DETECTOR_LINUX_H
#define JRE_SYSTEM_LIBRARY_PATH_DETECTOR_LINUX_H

#include "utility/path_detector/jre_system_library/JreSystemLibraryPathDetector.h"

class JreSystemLibraryPathDetectorLinux: public JreSystemLibraryPathDetector
{
public:
	JreSystemLibraryPathDetectorLinux(const std::string javaVersion);
};

#endif // JRE_SYSTEM_LIBRARY_PATH_DETECTOR_LINUX_H
