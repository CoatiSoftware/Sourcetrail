#include "ApplicationSettingsPrefiller.h"

#include "ApplicationSettings.h"
#include "MessageStatus.h"
#include "logging.h"
#include "utilityPathDetection.h"

void ApplicationSettingsPrefiller::prefillPaths(ApplicationSettings* settings)
{
	bool updated = false;

	updated |= prefillJavaRuntimePath(settings);
	updated |= prefillMavenExecutablePath(settings);
	updated |= prefillJreSystemLibraryPaths(settings);
	updated |= prefillCxxHeaderPaths(settings);
	updated |= prefillCxxFrameworkPaths(settings);

	if (updated)
	{
		settings->save();
	}
}

bool ApplicationSettingsPrefiller::prefillJavaRuntimePath(ApplicationSettings* settings)
{
	if (settings->getHasPrefilledJavaPath() && !settings->getJavaPath().empty())
	{
		return false;
	}

	LOG_INFO("Prefilling Java path");

	std::shared_ptr<CombinedPathDetector> javaPathDetector = utility::getJavaRuntimePathDetector();
	std::vector<FilePath> paths = javaPathDetector->getPaths();
	if (!paths.empty())
	{
		MessageStatus(L"Ran Java runtime path detection, found: " + paths.front().wstr()).dispatch();

		settings->setJavaPath(paths.front());
	}
	else
	{
		MessageStatus(L"Ran Java runtime path detection, no path found.").dispatch();
	}

	settings->setHasPrefilledJavaPath(true);
	return true;
}

bool ApplicationSettingsPrefiller::prefillJreSystemLibraryPaths(ApplicationSettings* settings)
{
	if (settings->getHasPrefilledJreSystemLibraryPaths())	 // allow empty
	{
		return false;
	}

	LOG_INFO("Prefilling JRE system library path");
	std::shared_ptr<CombinedPathDetector> jreSystemLibraryPathsDetector =
		utility::getJreSystemLibraryPathsDetector();
	std::vector<FilePath> paths = jreSystemLibraryPathsDetector->getPaths();
	if (!paths.empty())
	{
		MessageStatus(L"Ran JRE system library path detection, found: " + paths.front().wstr())
			.dispatch();

		settings->setJreSystemLibraryPaths(paths);
	}
	else
	{
		MessageStatus(L"Ran JRE system library path detection, no path found.").dispatch();
	}

	settings->setHasPrefilledJreSystemLibraryPaths(true);
	return true;
}

bool ApplicationSettingsPrefiller::prefillMavenExecutablePath(ApplicationSettings* settings)
{
	if (settings->getHasPrefilledMavenPath() && !settings->getMavenPath().empty())
	{
		return false;
	}

	LOG_INFO("Prefilling Maven path");
	std::shared_ptr<CombinedPathDetector> mavenPathDetector =
		utility::getMavenExecutablePathDetector();
	std::vector<FilePath> paths = mavenPathDetector->getPaths();
	if (!paths.empty())
	{
		MessageStatus(L"Ran Maven executable path detection, found: " + paths.front().wstr()).dispatch();

		settings->setMavenPath(paths.front());
	}
	else
	{
		MessageStatus(L"Ran Maven executable path detection, no path found.").dispatch();
	}

	settings->setHasPrefilledMavenPath(true);
	return true;
}

bool ApplicationSettingsPrefiller::prefillCxxHeaderPaths(ApplicationSettings* settings)
{
	if (settings->getHasPrefilledHeaderSearchPaths())	 // allow empty
	{
		return false;
	}

	LOG_INFO("Prefilling header search paths");
	std::shared_ptr<CombinedPathDetector> cxxHeaderDetector = utility::getCxxHeaderPathDetector();
	std::vector<FilePath> paths = cxxHeaderDetector->getPaths();
	if (!paths.empty())
	{
		MessageStatus(
			L"Ran C/C++ header path detection, found " + std::to_wstring(paths.size()) + L" path" +
			(paths.size() == 1 ? L"" : L"s"))
			.dispatch();

		settings->setHeaderSearchPaths(paths);
	}

	settings->setHasPrefilledHeaderSearchPaths(true);
	return true;
}

bool ApplicationSettingsPrefiller::prefillCxxFrameworkPaths(ApplicationSettings* settings)
{
	if (settings->getHasPrefilledFrameworkSearchPaths())	// allow empty
	{
		return false;
	}

	LOG_INFO("Prefilling framework search paths");
	std::shared_ptr<CombinedPathDetector> cxxFrameworkDetector =
		utility::getCxxFrameworkPathDetector();
	std::vector<FilePath> paths = cxxFrameworkDetector->getPaths();
	if (!paths.empty())
	{
		MessageStatus(
			L"Ran C/C++ framework path detection, found " + std::to_wstring(paths.size()) +
			L" path" + (paths.size() == 1 ? L"" : L"s"))
			.dispatch();

		settings->setFrameworkSearchPaths(paths);
	}

	settings->setHasPrefilledFrameworkSearchPaths(true);
	return true;
}
