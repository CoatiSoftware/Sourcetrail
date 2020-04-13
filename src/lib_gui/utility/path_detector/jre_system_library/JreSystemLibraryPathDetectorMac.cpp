#include "JreSystemLibraryPathDetectorMac.h"

#include "../java_runtime/JavaPathDetectorMac.h"

JreSystemLibraryPathDetectorMac::JreSystemLibraryPathDetectorMac(const std::string javaVersion)
	: JreSystemLibraryPathDetector(std::make_shared<JavaPathDetectorMac>(javaVersion))
{
}
