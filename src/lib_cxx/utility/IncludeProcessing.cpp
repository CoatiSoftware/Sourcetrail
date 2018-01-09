#include "utility/IncludeProcessing.h"

#include <set>
#include <unordered_set>

#include "utility/IncludeDirective.h"
#include "utility/file/FilePath.h"
#include "utility/file/FileSystem.h"
#include "utility/text/TextAccess.h"
#include "utility/utility.h"
#include "utility/utilityString.h"

namespace
{
	struct IncludeDirectiveComparator
	{
		bool operator()(const IncludeDirective& a, const IncludeDirective& b)
		{
			return a.getIncludedFile() < b.getIncludedFile();
		}
	};

	std::vector<std::vector<FilePath>> splitToQuantiles(
		const std::set<FilePath>& sourceFilePaths, 
		const size_t desiredQuantileCount)
	{
		size_t quantileCount = std::max<size_t>(1, std::min(desiredQuantileCount, sourceFilePaths.size()));

		std::vector<std::vector<FilePath>> quantiles;
		for (size_t i = 0; i < quantileCount; i++)
		{
			quantiles.push_back(std::vector<FilePath>());
		}

		int i = 0;
		for (const FilePath& sourceFilePath : sourceFilePaths)
		{
			quantiles[i % quantileCount].push_back(sourceFilePath);
			++i;
		}

		return quantiles;
	}
}

std::vector<IncludeDirective> IncludeProcessing::getUnresolvedIncludeDirectives(
	const std::set<FilePath>& sourceFilePaths,
	const std::set<FilePath>& indexedPaths,
	const std::set<FilePath>& headerSearchDirectories,
	const size_t desiredQuantileCount, std::function<void(float)> progress
)
{
	std::unordered_set<std::string> processedFilePaths;
	std::set<IncludeDirective, IncludeDirectiveComparator> unresolvedIncludeDirectives;

	std::vector<std::vector<FilePath>> quantiles = splitToQuantiles(sourceFilePaths, desiredQuantileCount);

	for (size_t i = 0; i < quantiles.size(); i++)
	{
		const std::vector<FilePath>& quantile = quantiles[i];

		progress(float(i) / quantiles.size());

		std::set<FilePath> unprocessedFilePaths(quantile.begin(), quantile.end());

		while (!unprocessedFilePaths.empty())
		{
			std::transform(
				unprocessedFilePaths.begin(), unprocessedFilePaths.end(),
				std::inserter(processedFilePaths, processedFilePaths.begin()),
				[](const FilePath& p){ return p.str(); }
			);

			std::set<FilePath> unprocessedFilePathsForNextIteration;

			for (const FilePath& filePath: unprocessedFilePaths)
			{
				for (const IncludeDirective& includeDirective: getIncludeDirectives(filePath))
				{
					const FilePath resolvedIncludePath = resolveIncludeDirective(includeDirective, headerSearchDirectories).makeCanonical();
					if (resolvedIncludePath.empty())
					{
						unresolvedIncludeDirectives.insert(includeDirective);
					}
					else if (processedFilePaths.find(resolvedIncludePath.str()) == processedFilePaths.end())
					{
						for (const FilePath& indexedPath: indexedPaths)
						{
							if (indexedPath.contains(resolvedIncludePath))
							{
								unprocessedFilePathsForNextIteration.insert(resolvedIncludePath);
								break;
							}
						}
					}
				}
			}

			unprocessedFilePaths = unprocessedFilePathsForNextIteration;
		}
	}

	std::vector<IncludeDirective> ret;

	for (const IncludeDirective& directive: unresolvedIncludeDirectives)
	{
		ret.push_back(directive);
	}

	progress(1.0f);

	return ret;
}

std::set<FilePath> IncludeProcessing::getHeaderSearchDirectories(
	const std::set<FilePath>& sourceFilePaths,
	const std::set<FilePath>& searchedPaths,
	const size_t desiredQuantileCount, std::function<void(float)> progress
)
{
	progress(0.0f);

	std::map<std::string, std::set<FilePath>> existingFilePaths;
	for (const FilePath& searchedPath : searchedPaths)
	{
		if (searchedPath.isDirectory())
		{
			for (const FilePath& filePath : FileSystem::getFilePathsFromDirectory(searchedPath.getCanonical()))
			{
				existingFilePaths[filePath.fileName()].insert(filePath);
			}
		}
		else
		{
			existingFilePaths[searchedPath.fileName()].insert(searchedPath);
		}
	}

	std::set<FilePath> headerSearchDirectories;
	std::unordered_set<std::string> processedFilePaths;
	std::vector<std::vector<FilePath>> quantiles = splitToQuantiles(sourceFilePaths, desiredQuantileCount);

	for (size_t i = 0; i < quantiles.size(); i++)
	{
		const std::vector<FilePath>& quantile = quantiles[i];

		progress(float(i) / quantiles.size());

		std::set<FilePath> unprocessedFilePaths(quantile.begin(), quantile.end());

		while (!unprocessedFilePaths.empty())
		{
			std::transform(
				unprocessedFilePaths.begin(), unprocessedFilePaths.end(),
				std::inserter(processedFilePaths, processedFilePaths.begin()),
				[](const FilePath& p) { return p.str(); }
			);

			std::set<FilePath> unprocessedFilePathsForNextIteration;

			for (const FilePath& unprocessedFilePath : unprocessedFilePaths)
			{
				for (const IncludeDirective& includeDirective : getIncludeDirectives(unprocessedFilePath))
				{
					const FilePath includedFilePath = includeDirective.getIncludedFile();

					FilePath foundIncludedPath;
					if (includedFilePath.isAbsolute())
					{
						foundIncludedPath = includedFilePath;
					}
					else
					{
						const FilePath relativeToIncludingPath = includeDirective.getIncludingFile().getParentDirectory().concatenate(includedFilePath);
						if (relativeToIncludingPath.exists())
						{
							foundIncludedPath = relativeToIncludingPath;
						}
						else
						{
							std::map<std::string, std::set<FilePath>>::const_iterator it = existingFilePaths.find(includedFilePath.fileName());
							if (it != existingFilePaths.end())
							{
								// TODO: handle the case where a file can be found by two different paths
								for (FilePath existingFilePath : it->second)
								{
									existingFilePath = existingFilePath.getParentDirectory();
									bool ok = true;
									{
										FilePath tempIncludedFilePath = includedFilePath.getParentDirectory();
										while (!tempIncludedFilePath.empty())
										{
											if (tempIncludedFilePath.fileName() == "..")
											{
												std::vector<FilePath> subDirectories = FileSystem::getDirectSubDirectories(existingFilePath);
												if (!subDirectories.empty())
												{
													existingFilePath = subDirectories.front();
												}
												else
												{
													ok = false;
													break;
												}
											}
											else
											{
												existingFilePath = existingFilePath.getParentDirectory();
											}
											tempIncludedFilePath = tempIncludedFilePath.getParentDirectory();
										}
									}
									if (ok)
									{
										foundIncludedPath = existingFilePath.getConcatenated(includedFilePath);
										if (foundIncludedPath.exists())
										{
											headerSearchDirectories.insert(existingFilePath);
											break;
										}
									}
								}
							}
						}
					}
					if (foundIncludedPath.exists())
					{
						if (processedFilePaths.find(foundIncludedPath.str()) == processedFilePaths.end())
						{
							for (const FilePath& searchedPath : searchedPaths)
							{
								if (searchedPath.contains(foundIncludedPath))
								{
									unprocessedFilePathsForNextIteration.insert(foundIncludedPath);
									break;
								}
							}
						}
					}
				}
			}

			unprocessedFilePaths = unprocessedFilePathsForNextIteration;
		}
	}

	progress(1.0f);

	return headerSearchDirectories;
}

std::vector<IncludeDirective> IncludeProcessing::getIncludeDirectives(const FilePath& filePath)
{
	if (filePath.exists())
	{
		return getIncludeDirectives(TextAccess::createFromFile(filePath));
	}
	return std::vector<IncludeDirective>();
}

std::vector<IncludeDirective> IncludeProcessing::getIncludeDirectives(std::shared_ptr<TextAccess> textAccess)
{
	std::vector<IncludeDirective> includeDirectives;

	const std::vector<std::string> lines = textAccess->getAllLines();
	for (size_t i = 0; i < lines.size(); i++)
	{
		const std::string lineTrimmedToHash = utility::trim(lines[i]);
		if (utility::isPrefix("#", lineTrimmedToHash))
		{
			const std::string lineTrimmedToInclude = utility::trim(lineTrimmedToHash.substr(1));
			if (utility::isPrefix("include", lineTrimmedToInclude))
			{
				std::string includeString = utility::substrBetween(lineTrimmedToInclude, "<", ">");
				bool usesBrackets = true;
				if (includeString.empty())
				{
					includeString = utility::substrBetween(lineTrimmedToInclude, "\"", "\"");
					usesBrackets = false;
				}

				if (!includeString.empty())
				{
					// lines are 1 based
					includeDirectives.push_back(IncludeDirective(FilePath(includeString), textAccess->getFilePath(), i + 1, usesBrackets));
				}
			}
		}
	}

	return includeDirectives;
}

FilePath IncludeProcessing::resolveIncludeDirective(
	const IncludeDirective& includeDirective, 
	const std::set<FilePath>& headerSearchDirectories
)
{
	const FilePath includedFilePath = includeDirective.getIncludedFile();

	{
		// check for an absolute include path
		if (includedFilePath.isAbsolute())
		{
			const FilePath resolvedIncludePath = includedFilePath;
			if (resolvedIncludePath.exists())
			{
				return includedFilePath;
			}
		}
	}

	{
		// check for an include path relative to the including path
		const FilePath resolvedIncludePath = includeDirective.getIncludingFile().getParentDirectory().concatenate(includedFilePath);
		if (resolvedIncludePath.exists())
		{
			return resolvedIncludePath;
		}
	}

	{
		// check for an include path relative to the header search directories
		for (const FilePath& headerSearchDirectory: headerSearchDirectories)
		{
			const FilePath resolvedIncludePath = headerSearchDirectory.getConcatenated(includedFilePath);
			if (resolvedIncludePath.exists())
			{
				return resolvedIncludePath;
			}
		}
	}

	return FilePath();
}
