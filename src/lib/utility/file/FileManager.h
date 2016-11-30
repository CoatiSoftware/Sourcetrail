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
		std::set<FilePath> allFiles;
		std::set<FilePath> addedFiles;
		std::set<FilePath> updatedFiles;
		std::set<FilePath> removedFiles;
	};

	FileManager();
	virtual ~FileManager();

	const std::vector<FilePath>& getSourcePaths() const;

	void setPaths(
		std::vector<FilePath> sourcePaths,
		std::vector<FilePath> headerPaths,
		std::vector<FilePath> excludePaths,
		std::vector<std::string> sourceExtensions
	);

	FileSets fetchFilePaths(const std::vector<FileInfo>& oldFileInfos);

	virtual bool hasFilePath(const FilePath& filePath) const;
	virtual bool hasSourceFilePath(const FilePath& filePath) const;

	virtual const FileInfo getFileInfo(const FilePath& filePath) const;

private:
	std::vector<FilePath> makeCanonical(const std::vector<FilePath>& filePaths);
	bool isExcluded(const FilePath& filePath) const;

	std::map<FilePath, FileInfo> m_files;

	std::vector<FilePath> m_sourcePaths;
	std::vector<FilePath> m_headerPaths;
	std::vector<FilePath> m_excludePaths;

	std::vector<std::string> m_sourceExtensions;

	std::set<FilePath> m_sourceFiles;
};

#endif // FILE_MANAGER_H
