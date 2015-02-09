#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <memory>
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
	std::vector<std::string> getAddedFilePaths() const;
	std::vector<std::string> getUpdatedFilePaths() const;
	std::vector<std::string> getRemovedFilePaths() const;

	virtual bool hasFilePath(const std::string& filePath) const;

private:
	std::vector<std::string> m_sourcePaths;
	std::vector<std::string> m_includePaths;
	std::vector<std::string> m_sourceExtensions;
	std::vector<std::string> m_includeExtensions;

	std::map<std::string, FileInfo> m_files;
	std::vector<std::string> m_addedFiles;
	std::vector<std::string> m_updatedFiles;
	std::vector<std::string> m_removedFiles;
};

#endif // FILE_MANAGER_H
