#ifndef UTILITY_MAVEN_H
#define UTILITY_MAVEN_H

#include "utility/file/FilePath.h"

namespace utility
{
	bool mavenGenerateSources(const FilePath& mavenPath, const FilePath& projectDirectoryPath);
	bool mavenCopyDependencies(const FilePath& mavenPath, const FilePath& projectDirectoryPath, const FilePath& outputDirectoryPath);
	std::vector<FilePath> mavenGetAllDirectoriesFromEffectivePom(const FilePath& mavenPath, const FilePath& projectDirectoryPath, bool addTestDirectories);
}

#endif // UTILITY_MAVEN_H
