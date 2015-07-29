#include "utility/file/FileSystem.h"

#include "boost/date_time.hpp"
#include "boost/filesystem.hpp"

std::vector<std::string> FileSystem::getFileNamesFromDirectory(
	const std::string& path, const std::vector<std::string>& extensions
){
	std::vector<std::string> files;

	if (boost::filesystem::is_directory(path))
	{
		boost::filesystem::recursive_directory_iterator it(path);
		boost::filesystem::recursive_directory_iterator endit;
		while (it != endit)
		{
			if (boost::filesystem::is_regular_file(*it) && hasExtension(it->path().string(), extensions))
			{
				files.push_back(it->path().generic_string());
			}
			++it;
		}
	}
	return files;
}

std::vector<std::string> FileSystem::getFileNamesFromDirectoryUpdatedAfter(
	const std::string& path, const std::vector<std::string>& extensions, const std::string& timeString
){
	std::vector<std::string> files;

	const boost::posix_time::ptime time = boost::posix_time::from_iso_string(timeString);

	if (boost::filesystem::is_directory(path))
	{
		boost::filesystem::recursive_directory_iterator it(path);
		boost::filesystem::recursive_directory_iterator endit;
		while (it != endit)
		{
			if (boost::filesystem::is_regular_file(*it) && hasExtension(it->path().string(), extensions))
			{
				std::time_t t = boost::filesystem::last_write_time(*it);
				boost::posix_time::ptime lastWriteTime = boost::posix_time::from_time_t(t);
				if (lastWriteTime >= time)
				{
					files.push_back(it->path().generic_string());
				}
			}
			++it;
		}
	}
	return files;
}

std::vector<FileInfo> FileSystem::getFileInfosFromPaths(
	const std::vector<FilePath>& paths, const std::vector<std::string>& fileExtensions
){
	std::vector<FileInfo> files;
	for (const FilePath& path: paths)
	{
		if (path.isDirectory())
		{
			boost::filesystem::recursive_directory_iterator it(path.path());
			boost::filesystem::recursive_directory_iterator endit;
			while (it != endit)
			{
				if (boost::filesystem::is_regular_file(*it) && hasExtension(it->path().string(), fileExtensions))
				{
					std::time_t t = boost::filesystem::last_write_time(*it);
					boost::posix_time::ptime lastWriteTime = boost::posix_time::from_time_t(t);
					files.push_back(FileInfo(it->path(), lastWriteTime));
				}
				++it;
			}
		}
		else if (path.exists() && path.hasExtension(fileExtensions))
		{
			std::time_t t = boost::filesystem::last_write_time(path.path());
			boost::posix_time::ptime lastWriteTime = boost::posix_time::from_time_t(t);
			files.push_back(FileInfo(path, lastWriteTime));
		}
	}
	return files;
}

std::string FileSystem::getTimeStringNow()
{
	return boost::posix_time::to_iso_string(boost::posix_time::second_clock::universal_time());
}

bool FileSystem::exists(const std::string& path)
{
	return boost::filesystem::exists(boost::filesystem::path(path));
}

std::string FileSystem::fileName(const std::string& path)
{
	return boost::filesystem::path(path).filename().generic_string();
}

std::string FileSystem::absoluteFilePath(const std::string& path)
{
	return boost::filesystem::absolute(boost::filesystem::path(path)).generic_string();
}

std::string FileSystem::extension(const std::string& path)
{
	return boost::filesystem::path(path).extension().generic_string();
}

std::string FileSystem::filePathWithoutExtension(const std::string& path)
{
	return boost::filesystem::path(path).replace_extension().generic_string();
}

bool FileSystem::hasExtension(const std::string& filepath, const std::vector<std::string>& extensions)
{
	boost::filesystem::path path(filepath);

	for (std::string extension : extensions)
	{
		if (path.extension() == extension)
		{
			return true;
		}
	}
	return false;
}

bool FileSystem::equivalent(const std::string& pathA, const std::string& pathB)
{
	if (exists(pathA) && exists(pathB))
	{
		return boost::filesystem::equivalent(boost::filesystem::path(pathA), boost::filesystem::path(pathB));
	}

	return boost::filesystem::path(pathA).compare(boost::filesystem::path(pathB)) == 0;
}
