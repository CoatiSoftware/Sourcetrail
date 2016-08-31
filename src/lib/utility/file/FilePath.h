#ifndef FILE_PATH_H
#define FILE_PATH_H

#include <string>

#include "boost/filesystem.hpp"

class FilePath
{
public:
	FilePath();
	FilePath(const char* filePath);
	FilePath(const std::string& filePath);
	FilePath(const boost::filesystem::path& filePath);

	boost::filesystem::path path() const;

	bool empty() const;
	bool exists() const;
	bool isDirectory() const;
	bool isAbsolute() const;

	FilePath parentDirectory() const;

	FilePath absolute() const;
	FilePath canonical() const;
	FilePath relativeTo(const FilePath& other) const;
	FilePath concat(const FilePath& other) const;
	FilePath expandEnvironmentVariables() const;

	bool contains(const FilePath& other) const;

	std::string str() const;
	std::string getBackslashedString() const;
	std::string fileName() const;

	std::string extension() const;
	FilePath withoutExtension() const;
	FilePath replaceExtension(const std::string& extension) const;
	bool hasExtension(const std::vector<std::string>& extensions) const;

	bool operator==(const FilePath& other) const;
	bool operator!=(const FilePath& other) const;
	bool operator<(const FilePath& other) const;

private:
	boost::filesystem::path m_path;

	mutable bool m_exists;
	mutable bool m_checkedExists;
};

#endif // FILE_PATH_H
