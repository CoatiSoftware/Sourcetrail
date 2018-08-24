#include "utility/file/FileSystem.h"

#include <set>

#include "boost/date_time.hpp"
#include "boost/filesystem.hpp"

#include "utility/utilityString.h"

std::vector<FilePath> FileSystem::getFilePathsFromDirectory(
	const FilePath& path, const std::vector<std::wstring>& extensions
){
	std::set<std::wstring> ext(extensions.begin(), extensions.end());
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

			if (boost::filesystem::is_regular_file(*it) && (ext.empty() || ext.find(it->path().extension().wstring()) != ext.end()))
			{
				files.push_back(FilePath(it->path().generic_wstring()));
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
	const std::vector<FilePath>& paths, const std::vector<std::wstring>& fileExtensions, bool followSymLinks
){
	std::set<std::wstring> ext;
	for (const std::wstring& e : fileExtensions)
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
					(ext.empty() || ext.find(utility::toLowerCase(it->path().extension().wstring())) != ext.end()))
				{
					boost::filesystem::path p = boost::filesystem::canonical(it->path());
					if (filePaths.find(p) != filePaths.end())
					{
						continue;
					}
					filePaths.insert(p);
					files.push_back(getFileInfoForPath(FilePath(it->path().wstring())));
				}
			}
		}
		else if (path.exists() && (ext.empty() || ext.find(utility::toLowerCase(path.extension())) != ext.end()))
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

std::set<FilePath> FileSystem::getSymLinkedDirectories(const FilePath& path)
{
	return getSymLinkedDirectories(std::vector<FilePath>{path});
}

std::set<FilePath> FileSystem::getSymLinkedDirectories(const std::vector<FilePath>& paths)
{
	std::set<boost::filesystem::path> symlinkDirs;

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
		files.insert(FilePath(p.wstring()));
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

bool FileSystem::remove(const FilePath& path)
{
	const bool ret = boost::filesystem::remove(path.getPath());
	path.recheckExists();
	return ret;
}

bool FileSystem::rename(const FilePath& from, const FilePath& to)
{
	if (!from.recheckExists() || to.recheckExists())
	{
		return false;
	}

	boost::filesystem::rename(from.getPath(), to.getPath());
	to.recheckExists();
	return true;
}

bool FileSystem::copyFile(const FilePath& from, const FilePath& to)
{
	if (!from.recheckExists() || to.recheckExists())
	{
		return false;
	}

	boost::filesystem::copy_file(from.getPath(), to.getPath());
	to.recheckExists();
	return true;
}

bool FileSystem::copy_directory(const FilePath& from, const FilePath& to)
{
	if (!from.recheckExists() || to.recheckExists())
	{
		return false;
	}

	boost::filesystem::copy_directory(from.getPath(), to.getPath());
	to.recheckExists();
	return true;
}

void FileSystem::createDirectory(const FilePath& path)
{
	boost::filesystem::create_directories(path.str());
	path.recheckExists();
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
				v.push_back(FilePath(dir->path().wstring()));
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
				v.push_back(FilePath(dir->path().wstring()));
			}
		}
	}

	return v;
}
