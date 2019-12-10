#include "utilityPathDetection.h"

#include "language_packages.h"

#include "logging.h"
#include "utilityApp.h"

#if BUILD_CXX_LANGUAGE_PACKAGE
#	include "CxxFrameworkPathDetector.h"
#	include "CxxHeaderPathDetector.h"
#	include "CxxVs10To14HeaderPathDetector.h"
#	include "CxxVs15HeaderPathDetector.h"
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

#if BUILD_JAVA_LANGUAGE_PACKAGE
#	include "JavaPathDetectorLinux.h"
#	include "JavaPathDetectorMac.h"
#	include "JavaPathDetectorWindows.h"
#	include "JreSystemLibraryPathDetectorLinux.h"
#	include "JreSystemLibraryPathDetectorMac.h"
#	include "JreSystemLibraryPathDetectorWindows.h"
#	include "MavenPathDetectorUnix.h"
#	include "MavenPathDetectorWindows.h"
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

std::shared_ptr<CombinedPathDetector> utility::getJavaRuntimePathDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();

#if BUILD_JAVA_LANGUAGE_PACKAGE
	switch (utility::getOsType())
	{
	case OS_WINDOWS:
		combinedDetector->addDetector(std::make_shared<JavaPathDetectorWindows>("1.8"));
		combinedDetector->addDetector(std::make_shared<JavaPathDetectorWindows>("9"));
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
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

	return combinedDetector;
}

std::shared_ptr<CombinedPathDetector> utility::getJreSystemLibraryPathsDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();

#if BUILD_JAVA_LANGUAGE_PACKAGE
	switch (utility::getOsType())
	{
	case OS_WINDOWS:
		combinedDetector->addDetector(std::make_shared<JreSystemLibraryPathDetectorWindows>("1.8"));
		combinedDetector->addDetector(std::make_shared<JreSystemLibraryPathDetectorWindows>("9"));
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
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

	return combinedDetector;
}

std::shared_ptr<CombinedPathDetector> utility::getMavenExecutablePathDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();

#if BUILD_JAVA_LANGUAGE_PACKAGE
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
#endif	  // BUILD_JAVA_LANGUAGE_PACKAGE

	return combinedDetector;
}

std::shared_ptr<CombinedPathDetector> utility::getCxxVsHeaderPathDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();

	if (utility::getOsType() != OS_WINDOWS)
	{
		return combinedDetector;
	}

#if BUILD_CXX_LANGUAGE_PACKAGE
	combinedDetector->addDetector(std::make_shared<CxxVs15HeaderPathDetector>());

	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2015, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2015, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2015, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2015, true, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2013, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2013, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2013, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2013, true, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2012, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2012, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2012, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2012, true, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2010, false, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2010, false, APPLICATION_ARCHITECTURE_X86_64));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2010, true, APPLICATION_ARCHITECTURE_X86_32));
	combinedDetector->addDetector(std::make_shared<CxxVs10To14HeaderPathDetector>(
		CxxVs10To14HeaderPathDetector::VISUAL_STUDIO_2010, true, APPLICATION_ARCHITECTURE_X86_64));
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE

	return combinedDetector;
}

std::shared_ptr<CombinedPathDetector> utility::getCxxHeaderPathDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = getCxxVsHeaderPathDetector();
#if BUILD_CXX_LANGUAGE_PACKAGE
	combinedDetector->addDetector(std::make_shared<CxxHeaderPathDetector>("clang"));
	combinedDetector->addDetector(std::make_shared<CxxHeaderPathDetector>("gcc"));
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
	return combinedDetector;
}

std::shared_ptr<CombinedPathDetector> utility::getCxxFrameworkPathDetector()
{
	std::shared_ptr<CombinedPathDetector> combinedDetector = std::make_shared<CombinedPathDetector>();
#if BUILD_CXX_LANGUAGE_PACKAGE
	combinedDetector->addDetector(std::make_shared<CxxFrameworkPathDetector>("clang"));
	combinedDetector->addDetector(std::make_shared<CxxFrameworkPathDetector>("gcc"));
#endif	  // BUILD_CXX_LANGUAGE_PACKAGE
	return combinedDetector;
}
