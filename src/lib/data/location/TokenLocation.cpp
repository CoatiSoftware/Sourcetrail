#include "data/location/TokenLocation.h"

#include "data/location/TokenLocationLine.h"

TokenLocation::TokenLocation(Id locationId, Id tokenId, TokenLocationLine* line, unsigned int columnNumber, bool isStart)
	: m_id(locationId)
	, m_tokenId(tokenId)
	, m_type(LOCATION_TOKEN)
	, m_line(line)
	, m_columnNumber(columnNumber)
	, m_other(nullptr)
	, m_isStart(isStart)
{
}

TokenLocation::TokenLocation(TokenLocation *other, TokenLocationLine* line, unsigned int columnNumber, bool isStart)
	: m_id(other->m_id)
	, m_tokenId(other->m_tokenId)
	, m_type(other->m_type)
	, m_line(line)
	, m_columnNumber(columnNumber)
	, m_other(other)
	, m_isStart(isStart)
{
}

TokenLocation::TokenLocation(const TokenLocation& other, TokenLocationLine* line)
	: m_id(other.m_id)
	, m_tokenId(other.m_tokenId)
	, m_type(other.m_type)
	, m_line(line)
	, m_columnNumber(other.m_columnNumber)
	, m_other(nullptr)
	, m_isStart(other.m_isStart)
{
}

TokenLocation::~TokenLocation()
{
}

bool TokenLocation::operator<(const TokenLocation& rhs) const
{
	return (
		getLineNumber() < rhs.getLineNumber() || (
			getLineNumber() == rhs.getLineNumber() &&
			getColumnNumber() < rhs.getColumnNumber()
		)
	);
}

bool TokenLocation::operator>(const TokenLocation& rhs) const
{
	return (
		getLineNumber() > rhs.getLineNumber() || (
			getLineNumber() == rhs.getLineNumber() &&
			getColumnNumber() > rhs.getColumnNumber()
		)
	);
}

Id TokenLocation::getId() const
{
	return m_id;
}

Id TokenLocation::getTokenId() const
{
	return m_tokenId;
}

LocationType TokenLocation::getType() const
{
	return m_type;
}

void TokenLocation::setType(LocationType type)
{
	m_type = type;

	if (m_other)
	{
		m_other->m_type = type;
	}
}

TokenLocationLine* TokenLocation::getTokenLocationLine() const
{
	return m_line;
}

TokenLocationFile* TokenLocation::getTokenLocationFile() const
{
	return m_line->getTokenLocationFile();
}

unsigned int TokenLocation::getColumnNumber() const
{
	return m_columnNumber;
}

unsigned int TokenLocation::getLineNumber() const
{
	return m_line->getLineNumber();
}

const FilePath& TokenLocation::getFilePath() const
{
	return m_line->getFilePath();
}

TokenLocation* TokenLocation::getOtherTokenLocation() const
{
	return m_other;
}

void TokenLocation::setOtherTokenLocation(TokenLocation* location)
{
	m_other = location;
}

TokenLocation* TokenLocation::getStartTokenLocation()
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

TokenLocation* TokenLocation::getEndTokenLocation()
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

const TokenLocation* TokenLocation::getStartTokenLocation() const
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

const TokenLocation* TokenLocation::getEndTokenLocation() const
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

bool TokenLocation::isStartTokenLocation() const
{
	return m_isStart;
}

bool TokenLocation::isEndTokenLocation() const
{
	return !m_isStart;
}

bool TokenLocation::isScopeTokenLocation() const
{
	return m_type == LOCATION_SCOPE;
}

bool TokenLocation::isFullTextSearchMatch() const
{
	return m_type == LOCATION_FULLTEXT;
}

std::ostream& operator<<(std::ostream& ostream, const TokenLocation& location)
{
	if ((&location)->isStartTokenLocation())
	{
		ostream << "<";
	}

	ostream << location.getColumnNumber() << ":[" << location.getTokenId() << "]";

	if ((&location)->isEndTokenLocation())
	{
		ostream << ">";
	}

	ostream << " ";
	return ostream;
}
