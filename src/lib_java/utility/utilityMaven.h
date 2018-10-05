#ifndef UTILITY_MAVEN_H
#define UTILITY_MAVEN_H

#include <vector>

class FilePath;

namespace utility
{
	std::wstring mavenGenerateSources(const FilePath& mavenPath, const FilePath& projectDirectoryPath);
	bool mavenCopyDependencies(const FilePath& mavenPath, const FilePath& projectDirectoryPath, const FilePath& outputDirectoryPath);
	std::vector<FilePath> mavenGetAllDirectoriesFromEffectivePom(const FilePath& mavenPath, const FilePath& projectDirectoryPath, bool addTestDirectories);
}

#endif // UTILITY_MAVEN_H
