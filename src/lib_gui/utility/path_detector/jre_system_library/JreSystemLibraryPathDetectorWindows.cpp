#include "JreSystemLibraryPathDetectorWindows.h"

#include "../java_runtime/JavaPathDetectorWindows.h"

JreSystemLibraryPathDetectorWindows::JreSystemLibraryPathDetectorWindows(const std::string javaVersion)
	: JreSystemLibraryPathDetector(std::make_shared<JavaPathDetectorWindows>(javaVersion))
{
}
