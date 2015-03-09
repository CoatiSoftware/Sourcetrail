#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

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

	std::set<std::string> getAddedFilePaths() const;
	std::set<std::string> getUpdatedFilePaths() const;
	std::set<std::string> getRemovedFilePaths() const;

	virtual bool hasFilePath(const std::string& filePath) const;
	virtual bool hasSourceExtension(const std::string& filePath) const;
	virtual bool hasIncludeExtension(const std::string& filePath) const;

private:
	std::vector<std::string> m_sourcePaths;
	std::vector<std::string> m_includePaths;
	std::vector<std::string> m_sourceExtensions;
	std::vector<std::string> m_includeExtensions;

	std::map<std::string, FileInfo> m_files;
	std::set<std::string> m_addedFiles;
	std::set<std::string> m_updatedFiles;
	std::set<std::string> m_removedFiles;
};

#endif // FILE_MANAGER_H
