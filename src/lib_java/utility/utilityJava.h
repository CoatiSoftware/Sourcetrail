#ifndef UTILITY_JAVA_H
#define UTILITY_JAVA_H

#include <string>
#include <set>
#include <vector>

class FilePath;
class SourceGroupSettingsWithClasspath;

namespace utility
{
	std::vector<std::wstring> getRequiredJarNames();
	std::string prepareJavaEnvironment();
	bool prepareJavaEnvironmentAndDisplayOccurringErrors();
	std::set<FilePath> fetchRootDirectories(const std::set<FilePath>& sourceFilePaths);
	std::vector<FilePath> getClassPath(
		std::shared_ptr<const SourceGroupSettingsWithClasspath> settings,
		const std::set<FilePath>& sourceFilePaths);
}

#endif // UTILITY_JAVA_H
