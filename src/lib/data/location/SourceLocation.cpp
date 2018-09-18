#include "SourceLocation.h"

#include "SourceLocationFile.h"

SourceLocation::SourceLocation(
	SourceLocationFile* file,
	LocationType type,
	Id locationId,
	std::vector<Id> tokenIds,
	size_t lineNumber,
	size_t columnNumber,
	bool isStart
)
	: m_file(file)
	, m_type(type)
	, m_locationId(locationId)
	, m_tokenIds(tokenIds)
	, m_lineNumber(lineNumber)
	, m_columnNumber(columnNumber)
	, m_other(nullptr)
	, m_isStart(isStart)
{
}

SourceLocation::SourceLocation(SourceLocation* other, size_t lineNumber, size_t columnNumber)
	: m_file(other->m_file)
	, m_type(other->m_type)
	, m_locationId(other->m_locationId)
	, m_tokenIds(other->m_tokenIds)
	, m_lineNumber(lineNumber)
	, m_columnNumber(columnNumber)
	, m_other(other)
	, m_isStart(!other->m_isStart)
{
	other->setOtherLocation(this);
}

SourceLocation::SourceLocation(const SourceLocation* other, SourceLocationFile* file)
	: m_file(file)
	, m_type(other->m_type)
	, m_locationId(other->m_locationId)
	, m_tokenIds(other->m_tokenIds)
	, m_lineNumber(other->m_lineNumber)
	, m_columnNumber(other->m_columnNumber)
	, m_other(nullptr)
	, m_isStart(other->m_isStart)
{
}

SourceLocation::~SourceLocation()
{
}

bool SourceLocation::operator==(const SourceLocation& rhs) const
{
	return (
		getLineNumber() == rhs.getLineNumber() &&
		getColumnNumber() == rhs.getColumnNumber() &&
		getLocationId() == rhs.getLocationId() &&
		getType() == rhs.getType()
	);
}

bool SourceLocation::operator<(const SourceLocation& rhs) const
{
	if (getLineNumber() != rhs.getLineNumber())
	{
		return getLineNumber() < rhs.getLineNumber();
	}

	if (getColumnNumber() != rhs.getColumnNumber())
	{
		return getColumnNumber() < rhs.getColumnNumber();
	}

	return getLocationId() < rhs.getLocationId();
}

bool SourceLocation::operator>(const SourceLocation& rhs) const
{
	if (getLineNumber() != rhs.getLineNumber())
	{
		return getLineNumber() > rhs.getLineNumber();
	}

	if (getColumnNumber() != rhs.getColumnNumber())
	{
		return getColumnNumber() > rhs.getColumnNumber();
	}

	return getLocationId() > rhs.getLocationId();
}

bool SourceLocation::contains(const SourceLocation& other) const
{
	const SourceLocation* start = getStartLocation();
	const SourceLocation* otherStart = other.getStartLocation();

	if (start->getLineNumber() > otherStart->getLineNumber())
	{
		return false;
	}

	if (start->getLineNumber() == otherStart->getLineNumber() &&
		start->getColumnNumber() > otherStart->getColumnNumber())
	{
		return false;
	}

	const SourceLocation* end = getEndLocation();
	const SourceLocation* otherEnd = other.getEndLocation();

	if (end->getLineNumber() < otherEnd->getLineNumber())
	{
		return false;
	}

	if (end->getLineNumber() == otherEnd->getLineNumber() &&
		end->getColumnNumber() < otherEnd->getColumnNumber())
	{
		return false;
	}

	return true;
}

SourceLocationFile* SourceLocation::getSourceLocationFile() const
{
	return m_file;
}

Id SourceLocation::getLocationId() const
{
	return m_locationId;
}

const std::vector<Id>& SourceLocation::getTokenIds() const
{
	return m_tokenIds;
}

LocationType SourceLocation::getType() const
{
	return m_type;
}

size_t SourceLocation::getColumnNumber() const
{
	return m_columnNumber;
}

size_t SourceLocation::getLineNumber() const
{
	return m_lineNumber;
}

const FilePath& SourceLocation::getFilePath() const
{
	return m_file->getFilePath();
}

const SourceLocation* SourceLocation::getOtherLocation() const
{
	return m_other;
}

void SourceLocation::setOtherLocation(SourceLocation* other)
{
	m_other = other;
}

const SourceLocation* SourceLocation::getStartLocation() const
{
	if (m_isStart)
	{
		return this;
	}
	else
	{
		return m_other;
	}
}

const SourceLocation* SourceLocation::getEndLocation() const
{
	if (!m_isStart)
	{
		return this;
	}
	else
	{
		return m_other;
	}
}

bool SourceLocation::isStartLocation() const
{
	return m_isStart;
}

bool SourceLocation::isEndLocation() const
{
	return !m_isStart;
}

bool SourceLocation::isScopeLocation() const
{
	return m_type == LOCATION_SCOPE;
}

bool SourceLocation::isFullTextSearchMatch() const
{
	return m_type == LOCATION_FULLTEXT_SEARCH;
}

std::wostream& operator<<(std::wostream& ostream, const SourceLocation& location)
{
	if (location.isStartLocation())
	{
		ostream << L'<';
	}

	ostream << location.getColumnNumber() << L":[ ";
	for (Id tokenId : location.getTokenIds())
	{
		ostream << L'\b' << tokenId << L' ';
	}

	ostream << L"\b]";

	if (location.isEndLocation())
	{
		ostream << L'>';
	}

	ostream << L' ';
	return ostream;
}
