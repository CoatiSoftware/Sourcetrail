#include "JreSystemLibraryPathDetectorMac.h"

#include "JavaPathDetectorMac.h"

JreSystemLibraryPathDetectorMac::JreSystemLibraryPathDetectorMac(const std::string javaVersion)
	: JreSystemLibraryPathDetector(std::make_shared<JavaPathDetectorMac>(javaVersion))
{
}
