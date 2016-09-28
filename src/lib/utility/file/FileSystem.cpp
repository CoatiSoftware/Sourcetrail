#include "utility/file/FileSystem.h"

#include <set>

#include "boost/date_time.hpp"
#include "boost/filesystem.hpp"

std::vector<std::string> FileSystem::getFileNamesFromDirectory(
	const std::string& path, const std::vector<std::string>& extensions
){
	std::set<std::string> ext(extensions.begin(), extensions.end());
	std::vector<std::string> files;

	if (boost::filesystem::is_directory(path))
	{
		boost::filesystem::recursive_directory_iterator it(path);
		boost::filesystem::recursive_directory_iterator endit;
		while (it != endit)
		{
			if (boost::filesystem::is_symlink(*it))
			{
				// check for self-referencing symlinks
				boost::filesystem::path p = boost::filesystem::read_symlink(*it);
				if (p.filename() == p.string() && p.filename() == it->path().filename())
				{
					++it;
					continue;
				}
			}

			if (boost::filesystem::is_regular_file(*it) && ext.find(it->path().extension().string()) != ext.end())
			{
				files.push_back(it->path().generic_string());
			}
			++it;
		}
	}
	return files;
}

FileInfo FileSystem::getFileInfoForPath(FilePath filePath)
{
	if (filePath.exists())
	{
		std::time_t t = boost::filesystem::last_write_time(filePath.path());
		boost::posix_time::ptime lastWriteTime = boost::posix_time::from_time_t(t);
		return FileInfo(filePath, lastWriteTime);
	}
	return FileInfo();
}

std::vector<FileInfo> FileSystem::getFileInfosFromPaths(
	const std::vector<FilePath>& paths, const std::vector<std::string>& fileExtensions, bool followSymLinks
){
	std::set<std::string> ext(fileExtensions.begin(), fileExtensions.end());

	std::set<boost::filesystem::path> symlinkDirs;
	std::set<boost::filesystem::path> filePaths;

	std::vector<FileInfo> files;

	for (const FilePath& path: paths)
	{
		if (path.isDirectory())
		{
			boost::filesystem::recursive_directory_iterator it(path.path(), boost::filesystem::symlink_option::recurse);
			boost::filesystem::recursive_directory_iterator endit;
			boost::system::error_code ec;
			for ( ; it != endit ; it.increment(ec) )
			{
				if (boost::filesystem::is_symlink(*it))
				{
					if (!followSymLinks)
					{
						it.no_push();
						continue;
					}

					// check for self-referencing symlinks
					boost::filesystem::path p = boost::filesystem::read_symlink(*it);
					if (p.filename() == p.string() && p.filename() == it->path().filename())
					{
						continue;
					}

					// check for duplicates when following directory symlinks
					if (boost::filesystem::is_directory(*it))
					{
						boost::filesystem::path absDir = boost::filesystem::canonical(p, it->path().parent_path());

						if (symlinkDirs.find(absDir) != symlinkDirs.end())
						{
							it.no_push();
							continue;
						}

						symlinkDirs.insert(absDir);
					}
				}

				if (boost::filesystem::is_regular_file(*it) &&
					(!ext.size() || ext.find(it->path().extension().string()) != ext.end()))
				{
					boost::filesystem::path p = boost::filesystem::canonical(it->path());
					if (filePaths.find(p) != filePaths.end())
					{
						continue;
					}
					filePaths.insert(p);

					std::time_t t = boost::filesystem::last_write_time(*it);
					boost::posix_time::ptime lastWriteTime = boost::posix_time::from_time_t(t);
					files.push_back(FileInfo(it->path(), lastWriteTime));
				}
			}
		}
		else if (path.exists() && (!ext.size() || ext.find(path.extension()) != ext.end()))
		{
			boost::filesystem::path p = boost::filesystem::canonical(path.path());
			if (filePaths.find(p) != filePaths.end())
			{
				continue;
			}
			filePaths.insert(p);

			std::time_t t = boost::filesystem::last_write_time(path.path());
			boost::posix_time::ptime lastWriteTime = boost::posix_time::from_time_t(t);
			files.push_back(FileInfo(path, lastWriteTime));
		}
	}

	return files;
}

TimePoint FileSystem::getLastWriteTime(const FilePath& filePath)
{
	boost::posix_time::ptime lastWriteTime;
	if (FileSystem::exists(filePath.str()))
	{
		std::time_t t = boost::filesystem::last_write_time(filePath.path());
		lastWriteTime = boost::posix_time::from_time_t(t);
	}
	return TimePoint(lastWriteTime);
}

std::string FileSystem::getTimeStringNow() // TODO: move to utility
{
	return boost::posix_time::to_iso_string(boost::posix_time::second_clock::universal_time());
}

bool FileSystem::exists(const FilePath& path)
{
	return boost::filesystem::exists(path.path());
}

bool FileSystem::remove(const FilePath& path)
{
	return boost::filesystem::remove(path.path());
}

bool FileSystem::rename(const FilePath& from, const FilePath& to)
{
	if (!from.exists() || to.exists())
	{
		return false;
	}

	boost::filesystem::rename(from.path(), to.path());
	return true;
}

bool FileSystem::copyFile(const FilePath& from, const FilePath& to)
{
	if (!from.exists() || to.exists())
	{
		return false;
	}

	boost::filesystem::copy_file(boost::filesystem::path(from.path()), boost::filesystem::path(to.path()));
	return true;
}

bool FileSystem::copy_directory(const FilePath& from, const FilePath& to)
{
	if (!from.exists() || to.exists())
	{
		return false;
	}

	boost::filesystem::copy_directory(boost::filesystem::path(from.path()),boost::filesystem::path(to.path()));
	return true;
}

void FileSystem::createDirectory(const FilePath& path)
{
	boost::filesystem::create_directories(path.str());
}

std::vector<FilePath> FileSystem::getSubDirectories(const FilePath &path)
{
	std::vector<FilePath> v;

	if (!path.exists())
	{
		return v;
	}

	for (boost::filesystem::recursive_directory_iterator end, dir(path.str()); dir != end; dir++)
	{
		if (boost::filesystem::is_directory(dir->path()))
		{
			v.push_back(FilePath(dir->path()));
		}
	}

	return v;
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

bool FileSystem::equivalent(const std::string& pathA, const std::string& pathB)
{
	if (exists(pathA) && exists(pathB))
	{
		return boost::filesystem::equivalent(boost::filesystem::path(pathA), boost::filesystem::path(pathB));
	}

	return boost::filesystem::path(pathA).compare(boost::filesystem::path(pathB)) == 0;
}
