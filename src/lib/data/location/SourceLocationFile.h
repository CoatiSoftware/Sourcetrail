#ifndef SOURCE_LOCATION_FILE_H
#define SOURCE_LOCATION_FILE_H

#include <functional>
#include <map>
#include <memory>
#include <ostream>
#include <set>

#include "LocationType.h"
#include "SourceLocation.h"
#include "FilePath.h"
#include "types.h"

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

	SourceLocationFile(const FilePath& filePath, const std::wstring& language, bool isWhole, bool isComplete, bool isIndexed);
	virtual ~SourceLocationFile();

	const FilePath& getFilePath() const;

	void setLanguage(const std::wstring& language);
	const std::wstring& getLanguage() const;

	void setIsWhole(bool isWhole);
	bool isWhole() const;

	void setIsComplete(bool isComplete);
	bool isComplete() const;

	void setIsIndexed(bool isIndexed);
	bool isIndexed() const;

	const std::multiset<std::shared_ptr<SourceLocation>, LocationComp>& getSourceLocations() const;

	size_t getSourceLocationCount() const;
	size_t getUnscopedStartLocationCount() const;

	SourceLocation* addSourceLocation(
		LocationType type, Id locationId, std::vector<Id> tokenIds,
		size_t startLineNumber, size_t startColumnNumber,
		size_t endLineNumber, size_t endColumnNumber);
	SourceLocation* addSourceLocationCopy(const SourceLocation* location);

	void copySourceLocations(std::shared_ptr<SourceLocationFile> file);

	SourceLocation* getSourceLocationById(Id locationId) const;

	void forEachSourceLocation(std::function<void(SourceLocation*)> func) const;
	void forEachStartSourceLocation(std::function<void(SourceLocation*)> func) const;
	void forEachEndSourceLocation(std::function<void(SourceLocation*)> func) const;

	std::shared_ptr<SourceLocationFile> getFilteredByLines(size_t firstLineNumber, size_t lastLineNumber) const;
	std::shared_ptr<SourceLocationFile> getFilteredByType(LocationType type) const;
	std::shared_ptr<SourceLocationFile> getFilteredByTypes(const std::vector<LocationType>& types) const;

private:
	const FilePath m_filePath;
	std::wstring m_language;
	bool m_isWhole;
	bool m_isComplete;
	bool m_isIndexed;

	std::multiset<std::shared_ptr<SourceLocation>, LocationComp> m_locations;
	std::map<Id, SourceLocation*> m_locationIndex;
};

std::wostream& operator<<(std::wostream& ostream, const SourceLocationFile& base);

#endif // SOURCE_LOCATION_FILE_H
