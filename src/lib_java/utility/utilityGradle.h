#ifndef UTILITY_GRADLE_H
#define UTILITY_GRADLE_H

#include <vector>

class FilePath;

namespace utility
{
	bool gradleCopyDependencies(const FilePath& projectDirectoryPath, const FilePath& outputDirectoryPath, bool addTestDependencies);
	std::vector<FilePath> gradleGetAllSourceDirectories(const FilePath& projectDirectoryPath, bool addTestDirectories);
}

#endif // UTILITY_GRADLE_H
