#ifndef INCLUDE_VALIDATION_H
#define INCLUDE_VALIDATION_H

#include <vector>

#include "utility/OrderedCache.h"

class FilePath;
class IncludeDirective;

class IncludeValidation
{
public:
	static std::vector<IncludeDirective> getUnresolvedIncludeDirectives(
		const std::vector<FilePath>& sourceFilePaths,
		const std::vector<FilePath>& indexedPaths,
		const std::vector<FilePath>& headerSearchDirectories,
		size_t quantileCount, std::function<void(float)> progress);
private:

	static std::vector<IncludeDirective> getIncludeDirectives(const FilePath& filePath);
	static FilePath resolveIncludeDirective(
		const IncludeDirective& includeDirective, 
		const std::vector<FilePath>& headerSearchDirectories,
		OrderedCache<FilePath, FilePath>& canonicalPathCache
	);
};

#endif // INCLUDE_VALIDATION_H
