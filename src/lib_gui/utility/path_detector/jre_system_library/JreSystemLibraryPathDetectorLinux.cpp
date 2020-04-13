#include "JreSystemLibraryPathDetectorLinux.h"

#include "../java_runtime/JavaPathDetectorLinux.h"

JreSystemLibraryPathDetectorLinux::JreSystemLibraryPathDetectorLinux(const std::string javaVersion)
	: JreSystemLibraryPathDetector(std::make_shared<JavaPathDetectorLinux>(javaVersion))
{
}
