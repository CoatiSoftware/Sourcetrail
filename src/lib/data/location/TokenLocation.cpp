#include "data/location/TokenLocation.h"

#include "data/location/TokenLocationLine.h"

TokenLocation::TokenLocation(Id tokenId, TokenLocationLine* line, unsigned int columnNumber, bool isStart)
	: TokenLocation(s_locationId++, tokenId, line, columnNumber, isStart)
{
}

TokenLocation::TokenLocation(Id id, Id tokenId, TokenLocationLine* line, unsigned int columnNumber, bool isStart)
	: m_id(id)
	, m_tokenId(tokenId)
	, m_line(line)
	, m_columnNumber(columnNumber)
	, m_other(nullptr)
	, m_isStart(isStart)
{
}

TokenLocation::~TokenLocation()
{
}

Id TokenLocation::getId() const
{
	return m_id;
}

Id TokenLocation::getTokenId() const
{
	return m_tokenId;
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

const std::string& TokenLocation::getFilePath() const
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

bool TokenLocation::isStartTokenLocation() const
{
	return m_isStart;
}

bool TokenLocation::isEndTokenLocation() const
{
	return !m_isStart;
}

std::shared_ptr<TokenLocation> TokenLocation::createPlainCopy(TokenLocationLine* line) const
{
	return std::shared_ptr<TokenLocation>(new TokenLocation(m_id, m_tokenId, line, m_columnNumber, m_isStart));
}

Id TokenLocation::s_locationId = 1;

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
