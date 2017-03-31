#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <map>
#include <set>
#include <vector>

#include "utility/file/FileInfo.h"

class FileManager
{
public:
	FileManager();
	virtual ~FileManager();

	void update(
		const std::vector<FilePath>& sourcePaths,
		const std::vector<FilePath>& excludePaths,
		const std::vector<std::string>& sourceExtensions
	);

	// returns a list of source paths (can be directories) specified in the project settings
	std::vector<FilePath> getSourcePaths() const;

	// checks if file is in non-excluded source directory
	bool hasSourceFilePath(const FilePath& filePath) const;

	// returns a list of paths to all files that reside in the non-excluded source paths
	std::set<FilePath> getAllSourceFilePaths() const;

private:
	std::vector<FilePath> makeCanonical(const std::vector<FilePath>& filePaths);
	bool isExcluded(const FilePath& filePath) const;

	std::vector<FilePath> m_sourcePaths;
	std::vector<FilePath> m_excludePaths;
	std::vector<std::string> m_sourceExtensions;

	std::set<FilePath> m_allSourceFilePaths;
};

#endif // FILE_MANAGER_H
