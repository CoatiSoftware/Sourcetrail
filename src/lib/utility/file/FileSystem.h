#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>

#include "utility/file/FileInfo.h"
#include "utility/TimePoint.h"

class FileSystem
{
public:
	static std::vector<std::string> getFileNamesFromDirectory(
		const std::string& path, const std::vector<std::string>& extensions);
	static std::vector<std::string> getFileNamesFromDirectoryUpdatedAfter(
		const std::string& path, const std::vector<std::string>& extensions, const std::string& timeString);

	static FileInfo getFileInfoForPath(FilePath filePath);

	static std::vector<FileInfo> getFileInfosFromPaths(
		const std::vector<FilePath>& paths, const std::vector<std::string>& fileExtensions);

	static TimePoint getLastWriteTime(const FilePath& filePath);
	static std::string getTimeStringNow();

	static void createDirectory(const FilePath& path);
	static std::vector<FilePath> getSubDirectories(const FilePath& path);

	static bool exists(const std::string& path);
	static std::string fileName(const std::string& path);
	static std::string absoluteFilePath(const std::string& path);
	static bool remove(const std::string& path);

	static bool copy_directory(const FilePath& from, const FilePath& to);

	static std::string extension(const std::string& path);
	static std::string filePathWithoutExtension(const std::string& path);
	static bool hasExtension(const std::string& filepath, const std::vector<std::string>& extensions);

	static bool equivalent(const std::string& pathA, const std::string& pathB);
};

#endif // FILE_SYSTEM_H
