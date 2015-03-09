#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <map>
#include <set>
#include <vector>

#include "FileInfo.h"

class FileManager
{
public:
	FileManager(
		std::vector<std::string> sourcePaths,
		std::vector<std::string> includePaths,
		std::vector<std::string> sourceExtensions,
		std::vector<std::string> includeExtensions
	);
	~FileManager();

	void reset();
	void fetchFilePaths();

	std::set<FilePath> getAddedFilePaths() const;
	std::set<FilePath> getUpdatedFilePaths() const;
	std::set<FilePath> getRemovedFilePaths() const;

	virtual bool hasFilePath(const FilePath& filePath) const;
	virtual bool hasSourceExtension(const FilePath& filePath) const;
	virtual bool hasIncludeExtension(const FilePath& filePath) const;

private:
	std::vector<std::string> m_sourcePaths;
	std::vector<std::string> m_includePaths;
	std::vector<std::string> m_sourceExtensions;
	std::vector<std::string> m_includeExtensions;

	std::map<FilePath, FileInfo> m_files;
	std::set<FilePath> m_addedFiles;
	std::set<FilePath> m_updatedFiles;
	std::set<FilePath> m_removedFiles;
};

#endif // FILE_MANAGER_H
