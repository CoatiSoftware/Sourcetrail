#include "utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorLinux.h"

#include "utility/path_detector/java_runtime/JavaPathDetectorLinux.h"

JreSystemLibraryPathDetectorLinux::JreSystemLibraryPathDetectorLinux(const std::string javaVersion)
	: JreSystemLibraryPathDetector(std::make_shared<JavaPathDetectorLinux>(javaVersion))
{
}

JreSystemLibraryPathDetectorLinux::~JreSystemLibraryPathDetectorLinux()
{
}
