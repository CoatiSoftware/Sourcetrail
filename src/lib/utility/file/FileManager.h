#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <map>
#include <set>
#include <string>
#include <vector>

class FilePath;
class FilePathFilter;

class FileManager
{
public:
	FileManager();
	virtual ~FileManager();

	void update(
		const std::vector<FilePath>& sourcePaths,
		const std::vector<FilePathFilter>& excludeFilters,
		const std::vector<std::wstring>& sourceExtensions
	);

	// returns a list of source paths (can be directories) specified in the project settings
	std::vector<FilePath> getSourcePaths() const;

	// checks if file is in non-excluded source directory
	bool hasSourceFilePath(const FilePath& filePath) const;

	// returns a list of paths to all files that reside in the non-excluded source paths
	std::set<FilePath> getAllSourceFilePaths() const;
	std::set<FilePath> getAllSourceFilePathsRelative(const FilePath& baseDirectory) const;

private:
	std::vector<FilePath> makeCanonical(const std::vector<FilePath>& filePaths);
	bool isExcluded(const FilePath& filePath) const;

	std::vector<FilePath> m_sourcePaths;
	std::vector<FilePathFilter> m_excludeFilters;
	std::vector<std::wstring> m_sourceExtensions;

	std::set<FilePath> m_allSourceFilePaths;
};

#endif // FILE_MANAGER_H
