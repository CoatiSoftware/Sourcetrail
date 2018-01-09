#include "utility/file/FileSystem.h"

#include <set>

#include "boost/date_time.hpp"
#include "boost/filesystem.hpp"

#include "utility/utilityString.h"

std::vector<FilePath> FileSystem::getFilePathsFromDirectory(
	const FilePath& path, const std::vector<std::string>& extensions
){
	std::set<std::string> ext(extensions.begin(), extensions.end());
	std::vector<FilePath> files;

	if (path.isDirectory())
	{
		boost::filesystem::recursive_directory_iterator it(path.getPath());
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

			if (boost::filesystem::is_regular_file(*it) && (ext.empty() || ext.find(it->path().extension().string()) != ext.end()))
			{
				files.push_back(FilePath(it->path().generic_string()));
			}
			++it;
		}
	}
	return files;
}

FileInfo FileSystem::getFileInfoForPath(const FilePath& filePath)
{
	if (filePath.exists())
	{
		return FileInfo(filePath, getLastWriteTime(filePath));
	}
	return FileInfo();
}

std::vector<FileInfo> FileSystem::getFileInfosFromPaths(
	const std::vector<FilePath>& paths, const std::vector<std::string>& fileExtensions, bool followSymLinks
){
	std::set<std::string> ext;
	for (const std::string& e : fileExtensions)
	{
		ext.insert(utility::toLowerCase(e));
	}

	std::set<boost::filesystem::path> symlinkDirs;
	std::set<boost::filesystem::path> filePaths;

	std::vector<FileInfo> files;

	for (const FilePath& path: paths)
	{
		if (path.isDirectory())
		{
			boost::filesystem::recursive_directory_iterator it(path.getPath(), boost::filesystem::symlink_option::recurse);
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
					(!ext.size() || ext.find(utility::toLowerCase(it->path().extension().string())) != ext.end()))
				{
					boost::filesystem::path p = boost::filesystem::canonical(it->path());
					if (filePaths.find(p) != filePaths.end())
					{
						continue;
					}
					filePaths.insert(p);
					files.push_back(getFileInfoForPath(FilePath(it->path())));
				}
			}
		}
		else if (path.exists() && (!ext.size() || ext.find(utility::toLowerCase(path.extension())) != ext.end()))
		{
			const FilePath canonicalPath = path.getCanonical();
			boost::filesystem::path p = canonicalPath.getPath();
			if (filePaths.find(p) != filePaths.end())
			{
				continue;
			}
			filePaths.insert(p);
			files.push_back(getFileInfoForPath(canonicalPath));
		}
	}

	return files;
}

std::set<FilePath> FileSystem::getSymLinkedDirectories(const std::vector<FilePath>& paths)
{
	std::set<boost::filesystem::path> symlinkDirs;
	std::set<boost::filesystem::path> filePaths;

	for (const FilePath& path: paths)
	{
		if (path.isDirectory())
		{
			boost::filesystem::recursive_directory_iterator it(path.getPath(), boost::filesystem::symlink_option::recurse);
			boost::filesystem::recursive_directory_iterator endit;
			boost::system::error_code ec;
			for ( ; it != endit ; it.increment(ec) )
			{
				if (boost::filesystem::is_symlink(*it))
				{
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
			}
		}
	}

	std::set<FilePath> files;
	for (auto& p : symlinkDirs)
	{
		files.insert(FilePath(p));
	}
	return files;
}

unsigned long long FileSystem::getFileByteSize(const FilePath& filePath)
{
	return boost::filesystem::file_size(filePath.getPath());
}

TimeStamp FileSystem::getLastWriteTime(const FilePath& filePath)
{
	boost::posix_time::ptime lastWriteTime;
	if (filePath.exists())
	{
		std::time_t t = boost::filesystem::last_write_time(filePath.getPath());
		lastWriteTime = boost::posix_time::from_time_t(t);
	}
	return TimeStamp(lastWriteTime);
}

std::string FileSystem::getTimeStringNow() // TODO: move to utility
{
	return boost::posix_time::to_iso_string(boost::posix_time::second_clock::universal_time());
}

bool FileSystem::remove(const FilePath& path)
{
	return boost::filesystem::remove(path.getPath());
}

bool FileSystem::rename(const FilePath& from, const FilePath& to)
{
	if (!from.exists() || to.exists())
	{
		return false;
	}

	boost::filesystem::rename(from.getPath(), to.getPath());
	return true;
}

bool FileSystem::copyFile(const FilePath& from, const FilePath& to)
{
	if (!from.exists() || to.exists())
	{
		return false;
	}

	boost::filesystem::copy_file(from.getPath(), to.getPath());
	return true;
}

bool FileSystem::copy_directory(const FilePath& from, const FilePath& to)
{
	if (!from.exists() || to.exists())
	{
		return false;
	}

	boost::filesystem::copy_directory(from.getPath(), to.getPath());
	return true;
}

void FileSystem::createDirectory(const FilePath& path)
{
	boost::filesystem::create_directories(path.str());
}

std::vector<FilePath> FileSystem::getDirectSubDirectories(const FilePath& path)
{
	std::vector<FilePath> v;

	if (path.exists() && path.isDirectory())
	{
		for (boost::filesystem::directory_iterator end, dir(path.str()); dir != end; dir++)
		{
			if (boost::filesystem::is_directory(dir->path()))
			{
				v.push_back(FilePath(dir->path()));
			}
		}
	}

	return v;
}

std::vector<FilePath> FileSystem::getRecursiveSubDirectories(const FilePath &path)
{
	std::vector<FilePath> v;

	if (path.exists() && path.isDirectory())
	{
		for (boost::filesystem::recursive_directory_iterator end, dir(path.str()); dir != end; dir++)
		{
			if (boost::filesystem::is_directory(dir->path()))
			{
				v.push_back(FilePath(dir->path()));
			}
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
