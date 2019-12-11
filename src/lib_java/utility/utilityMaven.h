#ifndef UTILITY_MAVEN_H
#define UTILITY_MAVEN_H

#include <string>
#include <vector>

class FilePath;

namespace utility
{
std::wstring mavenGenerateSources(
	const FilePath& mavenPath, const FilePath& settingsFilePath, const FilePath& projectDirectoryPath);
bool mavenCopyDependencies(
	const FilePath& mavenPath,
	const FilePath& settingsFilePath,
	const FilePath& projectDirectoryPath,
	const FilePath& outputDirectoryPath);
std::vector<FilePath> mavenGetAllDirectoriesFromEffectivePom(
	const FilePath& mavenPath,
	const FilePath& settingsFilePath,
	const FilePath& projectDirectoryPath,
	const FilePath& outputDirectoryPath,
	bool addTestDirectories);
}	 // namespace utility

#endif	  // UTILITY_MAVEN_H
