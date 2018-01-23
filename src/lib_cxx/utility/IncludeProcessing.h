#ifndef INCLUDE_PROCESSING_H
#define INCLUDE_PROCESSING_H

#include <memory>
#include <set>
#include <unordered_set>
#include <vector>

#include "utility/OrderedCache.h"

class FilePath;
class IncludeDirective;
class TextAccess;

class IncludeProcessing
{
public:
	static std::vector<IncludeDirective> getUnresolvedIncludeDirectives(
		const std::set<FilePath>& sourceFilePaths,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePath>& headerSearchDirectories,
		size_t quantileCount, std::function<void(float)> progress
	);

	static std::set<FilePath> getHeaderSearchDirectories(
		const std::set<FilePath>& sourceFilePaths,
		const std::set<FilePath>& searchedPaths,
		const std::set<FilePath>& currentHeaderSearchDirectories,
		const size_t desiredQuantileCount, std::function<void(float)> progress
	);

	static std::vector<IncludeDirective> getIncludeDirectives(const FilePath& filePath);

	static std::vector<IncludeDirective> getIncludeDirectives(std::shared_ptr<TextAccess> textAccess);

private:
	static std::vector<IncludeDirective> doGetUnresolvedIncludeDirectives(
		std::set<FilePath> filePathsToProcess,
		std::unordered_set<std::string>& processedFilePaths,
		const std::set<FilePath>& indexedPaths,
		const std::set<FilePath>& headerSearchDirectories
	);

	static FilePath resolveIncludeDirective(
		const IncludeDirective& includeDirective, 
		const std::set<FilePath>& headerSearchDirectories
	);

	IncludeProcessing() = delete;
};

#endif // INCLUDE_PROCESSING_H
