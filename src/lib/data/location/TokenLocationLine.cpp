#include "data/location/TokenLocationLine.h"

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "utility/logging/logging.h"

TokenLocationLine::TokenLocationLine(TokenLocationFile* file, unsigned int lineNumber)
	: m_file(file)
	, m_lineNumber(lineNumber)
{
}

TokenLocationLine::~TokenLocationLine()
{
}

const TokenLocationLine::TokenLocationMapType& TokenLocationLine::getTokenLocations() const
{
	return m_locations;
}

size_t TokenLocationLine::getTokenLocationCount() const
{
	return m_locations.size();
}

TokenLocationFile* TokenLocationLine::getTokenLocationFile() const
{
	return m_file;
}

const FilePath& TokenLocationLine::getFilePath() const
{
	return m_file->getFilePath();
}

unsigned int TokenLocationLine::getLineNumber() const
{
	return m_lineNumber;
}

TokenLocation* TokenLocationLine::addStartTokenLocation(Id locationId, Id tokenId, unsigned int columnNumber)
{
	std::shared_ptr<TokenLocation> locationPtr = std::make_shared<TokenLocation>(locationId, tokenId, this, columnNumber, true);
	m_locations.emplace(columnNumber, locationPtr);
	return locationPtr.get();
}

TokenLocation* TokenLocationLine::addEndTokenLocation(TokenLocation* start, unsigned int columnNumber)
{
	std::shared_ptr<TokenLocation> locationPtr = std::make_shared<TokenLocation>(start, this, columnNumber, false);
	start->setOtherTokenLocation(locationPtr.get());
	m_locations.emplace(columnNumber, locationPtr);
	return locationPtr.get();
}

void TokenLocationLine::removeTokenLocation(TokenLocation* location)
{
	TokenLocationMapType::iterator it = m_locations.find(location->getColumnNumber());

	while (it->first == location->getColumnNumber())
	{
		if (it->second.get() == location)
		{
			m_locations.erase(it);
			return;
		}
		it++;
	}

	LOG_ERROR("TokenLocation can't be removed, it's not part of the TokenLocationLine.");
}

TokenLocation* TokenLocationLine::getTokenLocationByIdAndType(Id id, LocationType type) const
{
	for (const TokenLocationPairType& p : m_locations)
	{
		if (p.second->getId() == id && p.second->getType() == type)
		{
			return p.second.get();
		}
	}
	return nullptr;
}

void TokenLocationLine::forEachTokenLocation(std::function<void(TokenLocation*)> func) const
{
	for (const TokenLocationPairType& location : m_locations)
	{
		func(location.second.get());
	}
}

void TokenLocationLine::forEachStartTokenLocation(std::function<void(TokenLocation*)> func) const
{
	for (const TokenLocationPairType& location : m_locations)
	{
		if (location.second->isStartTokenLocation())
		{
			func(location.second.get());
		}
	}
}

void TokenLocationLine::forEachEndTokenLocation(std::function<void(TokenLocation*)> func) const
{
	for (const TokenLocationPairType& location : m_locations)
	{
		if (location.second->isEndTokenLocation())
		{
			func(location.second.get());
		}
	}
}

TokenLocation* TokenLocationLine::addTokenLocationAsPlainCopy(const TokenLocation* location)
{
	std::shared_ptr<TokenLocation> locationPtr = std::make_shared<TokenLocation>(*location, this);
	m_locations.emplace(location->getColumnNumber(), locationPtr);
	return locationPtr.get();
}

std::ostream& operator<<(std::ostream& ostream, const TokenLocationLine& line)
{
	ostream << line.getLineNumber() << ":  ";
	line.forEachTokenLocation([&ostream](TokenLocation* l)
	{
		ostream << *l;
	});
	return ostream;
}
