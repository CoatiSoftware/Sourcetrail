#include "utility/utilityPathDetection.h"

#include "utility/logging/logging.h"

#include "utility/path_detector/java_runtime/JavaPathDetectorLinux.h"
#include "utility/path_detector/java_runtime/JavaPathDetectorMac.h"
#include "utility/path_detector/java_runtime/JavaPathDetectorWindows.h"

#include "utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorLinux.h"
#include "utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorMac.h"
#include "utility/path_detector/jre_system_library/JreSystemLibraryPathDetectorWindows.h"

#include "utility/path_detector/maven_executable/MavenPathDetectorUnix.h"
#include "utility/path_detector/maven_executable/MavenPathDetectorWindows.h"

#include "utility/path_detector/cxx_header/CxxFrameworkPathDetector.h"
#include "utility/path_detector/cxx_header/CxxHeaderPathDetector.h"
#include "utility/path_detector/cxx_header/CxxVsHeaderPathDetector.h"

#include "utility/utilityApp.h"

std::shared_ptr<CombinedPathDetector> utility::getJavaRuntimePathDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();

	switch (utility::getOsType())
	{
	case OS_WINDOWS:
		combinedDetector->addDetector(std::make_shared<JavaPathDetectorWindows>("1.8"));
		break;
	case OS_MAC:
		combinedDetector->addDetector(std::make_shared<JavaPathDetectorMac>("1.8"));
		break;
	case OS_LINUX:
		combinedDetector->addDetector(std::make_shared<JavaPathDetectorLinux>("1.8"));
		break;
	default:
		LOG_WARNING("No suitable Java Runtime path detector found");
		break;
	}

	return combinedDetector;
}

std::shared_ptr<CombinedPathDetector> utility::getJreSystemLibraryPathsDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();

	switch (utility::getOsType())
	{
	case OS_WINDOWS:
		combinedDetector->addDetector(std::make_shared<JreSystemLibraryPathDetectorWindows>("1.8"));
		break;
	case OS_MAC:
		combinedDetector->addDetector(std::make_shared<JreSystemLibraryPathDetectorMac>("1.8"));
		break;
	case OS_LINUX:
		combinedDetector->addDetector(std::make_shared<JreSystemLibraryPathDetectorLinux>("1.8"));
		break;
	default:
		LOG_WARNING("No suitable JRE system library path detector found");
		break;
	}

	return combinedDetector;
}

std::shared_ptr<CombinedPathDetector> utility::getMavenExecutablePathDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();

	switch (utility::getOsType())
	{
	case OS_WINDOWS:
		combinedDetector->addDetector(std::make_shared<MavenPathDetectorWindows>());
		break;
	case OS_MAC:
	case OS_LINUX:
		combinedDetector->addDetector(std::make_shared<MavenPathDetectorUnix>());
		break;
	default:
		LOG_WARNING("No suitable Maven path detector found");
		break;
	}

	return combinedDetector;
}

std::shared_ptr<CombinedPathDetector> utility::getCxxVsHeaderPathDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(14, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(14, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(14, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(14, true, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(12, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(12, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(12, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(12, true, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(11, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(11, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(11, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(11, true, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(9, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(9, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(9, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(9, true, APPLICATION_ARCHITECTURE_X86_64));
	return combinedDetector;
}

std::shared_ptr<CombinedPathDetector> utility::getCxxHeaderPathDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();
	combinedDetector->addDetector(std::make_shared<CxxHeaderPathDetector>("gcc"));
	combinedDetector->addDetector(std::make_shared<CxxHeaderPathDetector>("clang"));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(14, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(14, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(14, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(14, true, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(12, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(12, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(12, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(12, true, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(11, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(11, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(11, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(11, true, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(9, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(9, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(9, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVsHeaderPathDetector>(9, true, APPLICATION_ARCHITECTURE_X86_64));
	return combinedDetector;
}

std::shared_ptr<CombinedPathDetector> utility::getCxxFrameworkPathDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();
	combinedDetector->addDetector(std::make_shared<CxxFrameworkPathDetector>("gcc"));
	combinedDetector->addDetector(std::make_shared<CxxFrameworkPathDetector>("clang"));
	return combinedDetector;
}
