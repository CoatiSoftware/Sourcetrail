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
		const std::vector<FilePath>& classpathItems,
		bool useJreSystemLibrary,
		const std::set<FilePath>& sourceFilePaths);
	void setJavaHomeVariableIfNotExists();
}

#endif // UTILITY_JAVA_H
