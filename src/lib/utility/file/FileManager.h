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

	const std::vector<FilePath>& getSourcePaths() const;

	void setPaths(
		std::vector<FilePath> sourcePaths,
		std::vector<std::string> sourceExtensions,
		std::vector<std::string> includeExtensions
	);

	void clear();
	void fetchFilePaths(const std::vector<FileInfo>& oldFileInfos);

	std::set<FilePath> getAddedFilePaths() const;
	std::set<FilePath> getUpdatedFilePaths() const;
	std::set<FilePath> getRemovedFilePaths() const;

	virtual bool hasFilePath(const FilePath& filePath) const;
	virtual bool hasSourceExtension(const FilePath& filePath) const;
	virtual bool hasIncludeExtension(const FilePath& filePath) const;

	const FileInfo& getFileInfo(const FilePath& filePath) const;

private:
	std::vector<FileInfo> getFileInfosInProject() const;

	std::map<FilePath, FileInfo> m_files;

	std::vector<FilePath> m_sourcePaths;
	std::vector<std::string> m_sourceExtensions;
	std::vector<std::string> m_includeExtensions;

	std::set<FilePath> m_addedFiles;
	std::set<FilePath> m_updatedFiles;
	std::set<FilePath> m_removedFiles;
};

#endif // FILE_MANAGER_H
