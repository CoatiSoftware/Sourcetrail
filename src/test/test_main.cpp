#define CATCH_CONFIG_MAIN	 // This tells Catch to provide a main() function

#include "catch.hpp"
// IMPORTANT NOTE: removed signal listener for "EXCEPTION_ACCESS_VIOLATION" from catch source code
// because it interferes with the jni interface that emits such a signal on purpose

#include "ApplicationSettings.h"
#include "language_packages.h"
#include "utilityPathDetection.h"

struct EventListener: Catch::TestEventListenerBase
{
	using TestEventListenerBase::TestEventListenerBase;	   // inherit constructor

	void testRunStarting(const Catch::TestRunInfo& testRunInfo) override
	{
#ifdef __linux__
		const std::string homedir = getenv("HOME");

		if (!homedir.empty())
		{
			if (!ApplicationSettings::getInstance()->load(
					FilePath(homedir + "/.config/sourcetrail/ApplicationSettings.xml")))
			{
				std::cout << "no settings" << std::endl;
				return;
			}
		}
		else
		{
			std::cout << "no homedir" << std::endl;
			return;
		}
#else
		ApplicationSettings::getInstance()->load(FilePath(L"data/TestSettings.xml"));
#endif

#if BUILD_JAVA_LANGUAGE_PACKAGE
		if (ApplicationSettings::getInstance()->getJavaPath().empty())
		{
			std::shared_ptr<PathDetector> pathDetector = utility::getJavaRuntimePathDetector();
			const std::vector<FilePath> paths = pathDetector->getPaths();
			if (!paths.empty())
			{
				ApplicationSettings::getInstance()->setJavaPath(paths.front());
			}
		}
#endif
	}
};

CATCH_REGISTER_LISTENER(EventListener)
