#include "utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorWindows.h"

#include "utility/path_detector/java_runtime/JavaPathDetectorWindows.h"

JreSystemLibraryPathDetectorWindows::JreSystemLibraryPathDetectorWindows(const std::string javaVersion)
	: JreSystemLibraryPathDetector(std::make_shared<JavaPathDetectorWindows>(javaVersion))
{
}

JreSystemLibraryPathDetectorWindows::~JreSystemLibraryPathDetectorWindows()
{
}
