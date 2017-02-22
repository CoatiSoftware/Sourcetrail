#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <map>
#include <set>
#include <vector>

#include "utility/file/FileInfo.h"

class FileManager
{
public:
	struct FileSets
	{
		std::set<FilePath> addedFiles;
		std::set<FilePath> updatedFiles;
		std::set<FilePath> removedFiles;
		std::set<FilePath> allSourceFilePaths;
	};

	FileManager();
	virtual ~FileManager();

	void setPaths(
		std::vector<FilePath> sourcePaths,
		std::vector<FilePath> headerPaths,
		std::vector<FilePath> excludePaths,
		std::vector<std::string> sourceExtensions
	);

	FileSets fetchFilePaths(const std::vector<FileInfo>& oldFileInfos);

	// returns a list of source paths (can be directories) specified in the project settings
	std::vector<FilePath> getSourcePaths() const;

	// returns a list of paths to all files that reside in the non-excluded source paths
	std::set<FilePath> getSourceFilePaths() const;

	// checks if file is in non-excluded source directory
	virtual bool hasSourceFilePath(const FilePath& filePath) const;

private:
	std::vector<FilePath> makeCanonical(const std::vector<FilePath>& filePaths);
	bool isExcluded(const FilePath& filePath) const;

	std::vector<FilePath> m_sourcePaths;
	std::vector<FilePath> m_headerPaths;
	std::vector<FilePath> m_excludePaths;
	std::vector<std::string> m_sourceExtensions;

	std::map<FilePath, FileInfo> m_filesInfos;

	std::set<FilePath> m_sourceFilePaths;
};

#endif // FILE_MANAGER_H
