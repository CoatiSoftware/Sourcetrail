#include "utility/FileSystem.h"

#include "boost/filesystem.hpp"
#include "utility/logging/logging.h"

std::vector<std::string> FileSystem::getSourceFilesFromDirectory(
	const std::string& path, const std::vector<std::string>& extensions
)
{
	std::vector<std::string> files;
	if (boost::filesystem::is_directory(path))
	{
		boost::filesystem::recursive_directory_iterator it(path);
		boost::filesystem::recursive_directory_iterator endit;
		while(it != endit)
		{
			if (boost::filesystem::is_regular_file(*it) && isValidExtension(it->path().string(), extensions))
			{
				files.push_back(it->path().generic_string());
			}
			++it;
		}
	}
	return files;
}

bool FileSystem::isValidExtension(const std::string& filepath, const std::vector<std::string>& extensions)
{
	boost::filesystem::path path(filepath);
	for(std::string extension : extensions)
	{
		if(path.extension() == extension)
		{
			return true;
		}
	}
	return false;
}

bool FileSystem::exists(const std::string& path)
{
	return boost::filesystem::exists(boost::filesystem::path(path));
}
