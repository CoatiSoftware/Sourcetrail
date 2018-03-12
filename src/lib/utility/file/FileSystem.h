#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <set>
#include <string>
#include <vector>

#include "utility/file/FileInfo.h"
#include "utility/TimeStamp.h"

class FileSystem
{
public:
	static std::vector<FilePath> getFilePathsFromDirectory(
		const FilePath& path, const std::vector<std::wstring>& extensions = {});

	static FileInfo getFileInfoForPath(const FilePath& filePath);

	static std::vector<FileInfo> getFileInfosFromPaths(
		const std::vector<FilePath>& paths, const std::vector<std::wstring>& fileExtensions, bool followSymLinks = true);

	static std::set<FilePath> getSymLinkedDirectories(const FilePath& path);
	static std::set<FilePath> getSymLinkedDirectories(const std::vector<FilePath>& paths);

	static unsigned long long getFileByteSize(const FilePath& filePath);

	static TimeStamp getLastWriteTime(const FilePath& filePath);

	static bool remove(const FilePath& path);
	static bool rename(const FilePath& from, const FilePath& to);

	static bool copyFile(const FilePath& from, const FilePath& to);
	static bool copy_directory(const FilePath& from, const FilePath& to);

	static void createDirectory(const FilePath& path);
	static std::vector<FilePath> getDirectSubDirectories(const FilePath& path);
	static std::vector<FilePath> getRecursiveSubDirectories(const FilePath& path);
};

#endif // FILE_SYSTEM_H
