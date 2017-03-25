#ifndef SOURCE_LOCATION_COLLECTION_H
#define SOURCE_LOCATION_COLLECTION_H

#include <map>
#include <memory>
#include <ostream>
#include <vector>

#include "data/location/LocationType.h"
#include "utility/types.h"

class FilePath;
class SourceLocation;
class SourceLocationFile;

class SourceLocationCollection
{
public:
	SourceLocationCollection();
	virtual ~SourceLocationCollection();

	const std::map<FilePath, std::shared_ptr<SourceLocationFile>>& getSourceLocationFiles() const;

	size_t getSourceLocationCount() const;
	size_t getSourceLocationFileCount() const;

	std::shared_ptr<SourceLocationFile> getSourceLocationFileByPath(const FilePath& filePath) const;
	SourceLocation* getSourceLocationById(Id locationId) const;

	SourceLocation* addSourceLocation(
		LocationType type, Id locationId, std::vector<Id> tokenIds, const FilePath& filePath,
		size_t startLineNumber, size_t startColumnNumber,
		size_t endLineNumber, size_t endColumnNumber);
	SourceLocation* addSourceLocationCopy(SourceLocation* location);

	void addSourceLocationFile(std::shared_ptr<SourceLocationFile> file);

	void forEachSourceLocationFile(std::function<void(std::shared_ptr<SourceLocationFile>)> func) const;
	void forEachSourceLocation(std::function<void(SourceLocation*)> func) const;

private:
	SourceLocationFile* createSourceLocationFile(const FilePath& filePath);

	std::map<FilePath, std::shared_ptr<SourceLocationFile>> m_files;
};

std::ostream& operator<<(std::ostream& ostream, const SourceLocationCollection& base);

#endif // SOURCE_LOCATION_COLLECTION_H
