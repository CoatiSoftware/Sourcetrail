#ifndef FILE_REGISTER_H
#define FILE_REGISTER_H

#include <map>
#include <unordered_map>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <vector>

#include "utility/file/FileInfo.h"
#include "utility/file/FilePath.h"

class FileManager;

class FileRegister
{
public:
	explicit FileRegister(const FileManager* fileManager, bool randomizeParseOrder);

	void setFilePaths(const std::vector<FilePath>& filePaths);

	bool hasFilePath(const FilePath& filePath) const;
	const FileInfo getFileInfo(const FilePath& filePath) const;

	std::vector<FilePath> getUnparsedSourceFilePaths() const;

	bool hasIncludeFile(const FilePath& filePath) const;

	bool fileIsParsed(const FilePath& filePath) const;
	bool includeFileIsParsed(const FilePath& filePath) const;
	bool sourceFileIsParsed(const FilePath& filePath) const;

	FilePath consumeSourceFile();

	void markIncludeFileParsing(const FilePath& filePath);
	void markThreadFilesParsed();

	size_t getSourceFilesCount() const;
	size_t getParsedSourceFilesCount() const;

private:
	enum ParseState
	{
		STATE_UNPARSED,
		STATE_PARSING,
		STATE_PARSED
	};

	const FileManager* m_fileManager;
	bool m_randomizeParseOrder;

	mutable std::unordered_map<std::string, bool> m_projectFiles;
	mutable std::mutex m_projectFilesMutex;

	mutable std::unordered_map<std::string, FileInfo> m_projectFileInfos;
	mutable std::mutex m_projectFileInfosMutex;

	std::map<FilePath, ParseState> m_sourceFilePaths;
	std::map<FilePath, ParseState> m_includeFilePaths;

	std::map<std::thread::id, std::set<FilePath>> m_threadParsingFiles;

	mutable std::mutex m_sourceFileMutex;
	mutable std::mutex m_includeFileMutex;
	mutable std::mutex m_threadFileMutex;

	std::mutex m_consumeFileMutex;
};

#endif // FILE_REGISTER_H
