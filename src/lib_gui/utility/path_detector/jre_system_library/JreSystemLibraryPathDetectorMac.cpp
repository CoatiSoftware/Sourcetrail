#include "utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorMac.h"

#include "utility/path_detector/java_runtime/JavaPathDetectorMac.h"

JreSystemLibraryPathDetectorMac::JreSystemLibraryPathDetectorMac(const std::string javaVersion)
	: JreSystemLibraryPathDetector(std::make_shared<JavaPathDetectorMac>(javaVersion))
{
}

JreSystemLibraryPathDetectorMac::~JreSystemLibraryPathDetectorMac()
{
}
