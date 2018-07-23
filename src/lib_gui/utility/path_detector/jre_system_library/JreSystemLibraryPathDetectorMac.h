#ifndef JRE_SYSTEM_LIBRARY_PATH_DETECTOR_MAC_H
#define JRE_SYSTEM_LIBRARY_PATH_DETECTOR_MAC_H

#include "utility/path_detector/jre_system_library/JreSystemLibraryPathDetector.h"

class JreSystemLibraryPathDetectorMac: public JreSystemLibraryPathDetector
{
public:
	JreSystemLibraryPathDetectorMac(const std::string javaVersion);
};

#endif // JRE_SYSTEM_LIBRARY_PATH_DETECTOR_MAC_H
