#include "JreSystemLibraryPathDetectorLinux.h"

#include "JavaPathDetectorLinux.h"

JreSystemLibraryPathDetectorLinux::JreSystemLibraryPathDetectorLinux(const std::string javaVersion)
	: JreSystemLibraryPathDetector(std::make_shared<JavaPathDetectorLinux>(javaVersion))
{
}
