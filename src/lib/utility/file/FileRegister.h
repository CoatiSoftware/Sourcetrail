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
	FileRegister(const FileManager* fileManager, const std::vector<FilePath>& filePaths);

	const FileManager* getFileManager() const;

	const std::vector<FilePath>& getSourceFilePaths() const;

	bool includeFileIsParsing(const std::string& filePath) const;

	void markIncludeFileParsing(const std::string& filePath);
	void markParsingIncludeFilesParsed();

private:
	enum ParseState
	{
		STATE_UNPARSED,
		STATE_PARSING,
		STATE_PARSED
	};

	const FileManager* m_fileManager;

	std::vector<FilePath> m_sourceFilePaths;
	std::map<FilePath, ParseState> m_includeFilePaths;
};

#endif // FILE_REGISTER_H
