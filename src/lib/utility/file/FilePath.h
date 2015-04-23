#ifndef FILE_PATH_H
#define FILE_PATH_H

#include <string>

#include "boost/filesystem.hpp"

class FilePath
{
public:
	FilePath(const char* filePath);
	FilePath(const std::string& filePath);
	FilePath(const boost::filesystem::path& filePath);

	bool exists() const;

	std::string str() const;
	std::string absoluteStr() const;
	std::string fileName() const;

	std::string extension() const;
	FilePath withoutExtension() const;
	bool hasExtension(const std::vector<std::string>& extensions) const;

	bool operator==(const FilePath& other) const;
	bool operator!=(const FilePath& other) const;
	bool operator<(const FilePath& other) const;

private:
	void init();

	boost::filesystem::path m_path;
	bool m_exists;
};

#endif // FILE_PATH_H
