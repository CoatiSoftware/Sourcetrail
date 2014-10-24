#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>

class FileSystem
{
public:
	static std::vector<std::string> getSourceFilesFromDirectory( // TODO: Replace this with getFileNamesFromDirectory.
		const std::string& path, const std::vector<std::string>& extensions
	);

	static std::vector<std::string> getFileNamesFromDirectory(
		const std::string& path, const std::vector<std::string>& extensions
	);

	static bool exists(const std::string& path);
	static std::string fileName(const std::string& path);
	static std::string extension(const std::string& path);
	static std::string filePathWithoutExtension(const std::string& path);

private:
	static bool isValidExtension(const std::string& filepath, const std::vector<std::string>& extensions);
};

#endif // FILE_SYSTEM_H
