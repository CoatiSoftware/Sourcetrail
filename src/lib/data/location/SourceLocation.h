#ifndef SOURCE_LOCATION_H
#define SOURCE_LOCATION_H

#include <ostream>
#include <string>
#include <vector>

#include "LocationType.h"
#include "types.h"

class FilePath;
class SourceLocationFile;

class SourceLocation
{
public:
	SourceLocation(
		SourceLocationFile* file,
		LocationType type,
		Id locationId,
		std::vector<Id> tokenIds,
		size_t lineNumber,
		size_t columnNumber,
		bool isStart);
	SourceLocation(SourceLocation* other, size_t lineNumber, size_t columnNumber);
	SourceLocation(const SourceLocation* other, SourceLocationFile* file);
	virtual ~SourceLocation();

	bool operator==(const SourceLocation& rhs) const;
	bool operator<(const SourceLocation& rhs) const;
	bool operator>(const SourceLocation& rhs) const;

	bool contains(const SourceLocation& other) const;

	SourceLocationFile* getSourceLocationFile() const;

	Id getLocationId() const;
	const std::vector<Id>& getTokenIds() const;
	LocationType getType() const;

	size_t getColumnNumber() const;
	size_t getLineNumber() const;
	const FilePath& getFilePath() const;

	const SourceLocation* getOtherLocation() const;
	void setOtherLocation(SourceLocation* other);

	const SourceLocation* getStartLocation() const;
	const SourceLocation* getEndLocation() const;

	bool isStartLocation() const;
	bool isEndLocation() const;

	bool isScopeLocation() const;
	bool isFullTextSearchMatch() const;

private:
	SourceLocationFile* m_file;

	LocationType m_type;

	const Id m_locationId;
	const std::vector<Id> m_tokenIds;

	const size_t m_lineNumber;
	const size_t m_columnNumber;

	SourceLocation* m_other;
	const bool m_isStart;
};

std::wostream& operator<<(std::wostream& ostream, const SourceLocation& location);

#endif	  // SOURCE_LOCATION_H
