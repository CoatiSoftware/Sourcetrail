#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>

#include "utility/file/FileInfo.h"
#include "utility/TimePoint.h"

class FileSystem
{
public:
	static std::vector<FilePath> getFilePathsFromDirectory(
		const FilePath& path, const std::vector<std::string>& extensions);

	static FileInfo getFileInfoForPath(const FilePath& filePath);

	static std::vector<FileInfo> getFileInfosFromPaths(
		const std::vector<FilePath>& paths, const std::vector<std::string>& fileExtensions, bool followSymLinks = true);

	static TimePoint getLastWriteTime(const FilePath& filePath);
	static std::string getTimeStringNow();

	static bool exists(const FilePath& path);
	static bool remove(const FilePath& path);
	static bool rename(const FilePath& from, const FilePath& to);

	static bool copyFile(const FilePath& from, const FilePath& to);
	static bool copy_directory(const FilePath& from, const FilePath& to);

	static void createDirectory(const FilePath& path);
	static std::vector<FilePath> getSubDirectories(const FilePath& path);

	static std::string fileName(const std::string& path);
	static std::string absoluteFilePath(const std::string& path);

	static std::string extension(const std::string& path);
	static std::string filePathWithoutExtension(const std::string& path);
};

#endif // FILE_SYSTEM_H
