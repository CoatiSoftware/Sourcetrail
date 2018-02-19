#ifndef SOURCE_LOCATION_FILE_H
#define SOURCE_LOCATION_FILE_H

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <set>

#include "data/location/LocationType.h"
#include "data/location/SourceLocation.h"
#include "utility/file/FilePath.h"
#include "utility/types.h"

class SourceLocationFile
{
public:
	struct LocationComp
	{
		bool operator()(const std::shared_ptr<SourceLocation>& lhs, const std::shared_ptr<SourceLocation>& rhs) const
		{
			return *(lhs.get()) < *(rhs.get());
		}
	};

	SourceLocationFile(const FilePath& filePath, bool isWhole, bool isComplete);
	virtual ~SourceLocationFile();

	const FilePath& getFilePath() const;

	void setIsWhole(bool isWhole);
	bool isWhole() const;

	void setIsComplete(bool isComplete);
	bool isComplete() const;

	const std::multiset<std::shared_ptr<SourceLocation>, LocationComp>& getSourceLocations() const;

	size_t getSourceLocationCount() const;
	size_t getUnscopedStartLocationCount() const;

	SourceLocation* addSourceLocation(
		LocationType type, Id locationId, std::vector<Id> tokenIds,
		size_t startLineNumber, size_t startColumnNumber,
		size_t endLineNumber, size_t endColumnNumber);
	SourceLocation* addSourceLocationCopy(const SourceLocation* location);

	SourceLocation* getSourceLocationById(Id locationId) const;

	void forEachSourceLocation(std::function<void(SourceLocation*)> func) const;
	void forEachStartSourceLocation(std::function<void(SourceLocation*)> func) const;
	void forEachEndSourceLocation(std::function<void(SourceLocation*)> func) const;

	std::shared_ptr<SourceLocationFile> getFilteredByLines(size_t firstLineNumber, size_t lastLineNumber) const;
	std::shared_ptr<SourceLocationFile> getFilteredByType(LocationType type) const;

private:
	const FilePath m_filePath;
	bool m_isWhole;
	bool m_isComplete;

	std::multiset<std::shared_ptr<SourceLocation>, LocationComp> m_locations;
	std::map<Id, SourceLocation*> m_locationIndex;
};

std::wostream& operator<<(std::wostream& ostream, const SourceLocationFile& base);

#endif // SOURCE_LOCATION_FILE_H
