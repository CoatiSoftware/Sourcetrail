#ifndef FILE_PATH_H
#define FILE_PATH_H

#include <string>
#include <vector>
#include <memory>

namespace boost 
{
	namespace filesystem 
	{
		class path;
	}
}

class FilePath
{
public:
	FilePath();
	explicit FilePath(const std::string& filePath);
	explicit FilePath(const std::wstring& filePath);
	FilePath(const FilePath& other);
	FilePath(FilePath&& other);
	FilePath(const std::string& filePath, const std::string& base);
	~FilePath();

	boost::filesystem::path getPath() const;

	bool empty() const;
	bool exists() const;
	bool recheckExists() const;
	bool isDirectory() const;
	bool isAbsolute() const;

	FilePath getParentDirectory() const;

	FilePath& makeAbsolute();
	FilePath getAbsolute() const;
	FilePath& makeCanonical();
	FilePath getCanonical() const;
	FilePath& makeRelativeTo(const FilePath& other);
	FilePath getRelativeTo(const FilePath& other) const;
	FilePath& concatenate(const FilePath& other);
	FilePath getConcatenated(const FilePath& other) const;
	FilePath& concatenate(const std::wstring& other);
	FilePath getConcatenated(const std::wstring& other) const;
	std::vector<FilePath> expandEnvironmentVariables() const;

	bool contains(const FilePath& other) const;

	std::string str() const;
	std::wstring wstr() const;
	std::string getBackslashedString() const;
	std::string fileName() const;
	std::wstring wFileName() const;

	std::string extension() const;
	FilePath withoutExtension() const;
	FilePath replaceExtension(const std::string& extension) const;
	bool hasExtension(const std::vector<std::string>& extensions) const;

	FilePath& operator=(const FilePath& other);
	FilePath& operator=(FilePath&& other);
	bool operator==(const FilePath& other) const;
	bool operator!=(const FilePath& other) const;
	bool operator<(const FilePath& other) const;

private:
	std::unique_ptr<boost::filesystem::path> m_path;

	mutable bool m_exists;
	mutable bool m_checkedExists;
	mutable bool m_isDirectory;
	mutable bool m_checkedIsDirectory;
	mutable bool m_canonicalized;
};

#endif // FILE_PATH_H
