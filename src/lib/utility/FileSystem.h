#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <string>
#include <vector>

class FileSystem
{
public:
	static std::vector<std::string> getSourceFilesFromDirectory(
		const std::string& path, const std::vector<std::string>& extensions
	);
	static bool exists(const std::string& path);
private:
	static bool isValidExtension(const std::string& filepath, const std::vector<std::string>& extensions);
};

#endif // FILE_SYSTEM_H
