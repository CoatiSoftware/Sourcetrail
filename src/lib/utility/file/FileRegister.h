#ifndef FILE_REGISTER_H
#define FILE_REGISTER_H

#include <map>
#include <string>
#include <vector>

#include "utility/file/FilePath.h"

class FileManager;

class FileRegister
{
public:
	explicit FileRegister(const FileManager* fileManager);

	const FileManager* getFileManager() const;

	void setFilePaths(const std::vector<FilePath>& filePaths);

	std::vector<FilePath> getUnparsedSourceFilePaths() const;
	std::vector<FilePath> getUnparsedIncludeFilePaths() const;

	bool fileIsParsed(const FilePath& filePath) const;

	bool includeFileIsParsing(const FilePath& filePath) const;
	bool includeFileIsParsed(const FilePath& filePath) const;

	void markSourceFileParsed(const std::string& filePath);
	void markIncludeFileParsing(const std::string& filePath);
	void markParsingIncludeFilesParsed();

	size_t getFilesCount() const;
	size_t getSourceFilesCount() const;
	size_t getParsedFilesCount() const;
	size_t getParsedSourceFilesCount() const;

private:
	enum ParseState
	{
		STATE_UNPARSED,
		STATE_PARSING,
		STATE_PARSED
	};

	std::vector<FilePath> getUnparsedFilePaths(const std::map<FilePath, ParseState> filePaths) const;

	const FileManager* m_fileManager;

	std::map<FilePath, ParseState> m_sourceFilePaths;
	std::map<FilePath, ParseState> m_includeFilePaths;
};

#endif // FILE_REGISTER_H
