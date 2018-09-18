#include "JreSystemLibraryPathDetectorWindows.h"

#include "JavaPathDetectorWindows.h"

JreSystemLibraryPathDetectorWindows::JreSystemLibraryPathDetectorWindows(const std::string javaVersion)
	: JreSystemLibraryPathDetector(std::make_shared<JavaPathDetectorWindows>(javaVersion))
{
}
