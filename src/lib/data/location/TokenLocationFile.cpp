#include "data/location/TokenLocationFile.h"

#include <set>

#include "utility/logging/logging.h"
#include "utility/types.h"

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationLine.h"

TokenLocationFile::TokenLocationFile(const FilePath& filePath)
	: isWholeCopy(false)
	, m_filePath(filePath)
{
}

TokenLocationFile::~TokenLocationFile()
{
}

const TokenLocationFile::TokenLocationLineMapType& TokenLocationFile::getTokenLocationLines() const
{
	return m_lines;
}

size_t TokenLocationFile::getTokenLocationLineCount() const
{
	return m_lines.size();
}

size_t TokenLocationFile::getUnscopedStartTokenLocationCount() const
{
	size_t count = 0;
	for (const TokenLocationLinePairType& line : m_lines)
	{
		line.second->forEachStartTokenLocation(
			[&count](TokenLocation* location)
			{
				if (!location->isScopeTokenLocation())
				{
					count++;
				}
			}
		);
	}
	return count;
}

const FilePath& TokenLocationFile::getFilePath() const
{
	return m_filePath;
}

TokenLocation* TokenLocationFile::addTokenLocation(
	Id locationId, Id tokenId,
	unsigned int startLineNumber, unsigned int startColumnNumber,
	unsigned int endLineNumber, unsigned int endColumnNumber)
{
	TokenLocationLine* line = createTokenLocationLine(startLineNumber);
	TokenLocation* start = line->addStartTokenLocation(locationId, tokenId, startColumnNumber);

	if (startLineNumber != endLineNumber)
	{
		line = createTokenLocationLine(endLineNumber);
	}

	line->addEndTokenLocation(start, endColumnNumber);

	return start;
}

void TokenLocationFile::removeTokenLocation(TokenLocation* location)
{
	TokenLocationLine* line = location->getTokenLocationLine();

	TokenLocation* otherLocation = location->getOtherTokenLocation();
	TokenLocationLine* otherLine = otherLocation->getTokenLocationLine();

	line->removeTokenLocation(location);
	if (!line->getTokenLocationCount())
	{
		m_lines.erase(line->getLineNumber());
	}

	otherLine->removeTokenLocation(otherLocation);
	if (!otherLine->getTokenLocationCount())
	{
		m_lines.erase(otherLine->getLineNumber());
	}
}

TokenLocationLine* TokenLocationFile::findTokenLocationLineByNumber(unsigned int lineNumber) const
{
	return findTokenLocationLine(lineNumber);
}

void TokenLocationFile::forEachTokenLocationLine(std::function<void(TokenLocationLine*)> func) const
{
	for (const TokenLocationLinePairType& line : m_lines)
	{
		func(line.second.get());
	}
}

void TokenLocationFile::forEachTokenLocation(std::function<void(TokenLocation*)> func) const
{
	for (const TokenLocationLinePairType& line : m_lines)
	{
		line.second->forEachTokenLocation(func);
	}
}

void TokenLocationFile::forEachStartTokenLocation(std::function<void(TokenLocation*)> func) const
{
	for (const TokenLocationLinePairType& line : m_lines)
	{
		line.second->forEachStartTokenLocation(func);
	}
}

void TokenLocationFile::forEachEndTokenLocation(std::function<void(TokenLocation*)> func) const
{
	for (const TokenLocationLinePairType& line : m_lines)
	{
		line.second->forEachEndTokenLocation(func);
	}
}

TokenLocation* TokenLocationFile::addTokenLocationAsPlainCopy(const TokenLocation* location)
{
	unsigned int lineNumber = location->getTokenLocationLine()->getLineNumber();
	TokenLocationLine* line = createTokenLocationLine(lineNumber);

	// Check whether this location was already added or if the other TokenLocation was added.
	TokenLocation* otherLocation = line->getTokenLocationById(location->getId());
	if (otherLocation)
	{
		if (otherLocation->isStartTokenLocation() == location->isStartTokenLocation())
		{
			// The location was already added.
			return otherLocation;
		}
	}
	else
	{
		// Look for the other location in it's line.
		unsigned int otherLineNumber = location->getOtherTokenLocation()->getTokenLocationLine()->getLineNumber();
		if (lineNumber != otherLineNumber)
		{
			TokenLocationLine* otherLine = findTokenLocationLine(otherLineNumber);
			if (otherLine)
			{
				otherLocation = otherLine->getTokenLocationById(location->getId());
			}
		}
	}

	TokenLocation* copy = line->addTokenLocationAsPlainCopy(location);

	// If the other location was added before, then link them with each other.
	if (otherLocation)
	{
		otherLocation->setOtherTokenLocation(copy);
		copy->setOtherTokenLocation(otherLocation);
	}

	return copy;
}

std::shared_ptr<TokenLocationFile> TokenLocationFile::getFilteredByLines(unsigned int firstLineNumber, unsigned int lastLineNumber) const
{
	std::shared_ptr<TokenLocationFile> ret = std::make_shared<TokenLocationFile>(getFilePath().str());

	if (getTokenLocationLines().size() == 0)
	{
		return ret;
	}

	uint endLineNumber = getTokenLocationLines().rbegin()->first;
	std::set<Id> addedLocationIds;
	for (uint i = firstLineNumber; i <= endLineNumber; i++)
	{
		TokenLocationLine* locationLine = findTokenLocationLineByNumber(i);
		if (!locationLine)
		{
			continue;
		}

		if (locationLine->getLineNumber() <= lastLineNumber)
		{
			locationLine->forEachTokenLocation(
				[&](TokenLocation* tokenLocation) -> void
			{
				const Id tokenId = tokenLocation->getId();
				if (addedLocationIds.find(tokenId) == addedLocationIds.end())
				{
					ret->addTokenLocationAsPlainCopy(tokenLocation->getStartTokenLocation());
					ret->addTokenLocationAsPlainCopy(tokenLocation->getEndTokenLocation());
					addedLocationIds.insert(tokenId);
				}
			}
			);
		}
		else
		{
			// Save start locations of TokenLocations that span accross the line range.
			locationLine->forEachTokenLocation(
				[&](TokenLocation* tokenLocation) -> void
			{
				if (tokenLocation->isEndTokenLocation() &&
					tokenLocation->getStartTokenLocation()->getLineNumber() < firstLineNumber)
				{
					ret->addTokenLocationAsPlainCopy(tokenLocation->getStartTokenLocation());
					ret->addTokenLocationAsPlainCopy(tokenLocation->getEndTokenLocation());
				}
			}
			);
		}
	}

	return ret;
}

TokenLocationLine* TokenLocationFile::findTokenLocationLine(unsigned int lineNumber) const
{
	TokenLocationLineMapType::const_iterator it = m_lines.find(lineNumber);

	if (it != m_lines.end())
	{
		return it->second.get();
	}

	return nullptr;
}

TokenLocationLine* TokenLocationFile::createTokenLocationLine(unsigned int lineNumber)
{
	TokenLocationLine* line = findTokenLocationLine(lineNumber);

	if (line)
	{
		return line;
	}

	std::shared_ptr<TokenLocationLine> linePtr = std::make_shared<TokenLocationLine>(this, lineNumber);
	m_lines.emplace(lineNumber, linePtr);
	return linePtr.get();
}

std::ostream& operator<<(std::ostream& ostream, const TokenLocationFile& file)
{
	ostream << "file \"" << file.getFilePath().str() << "\"\n";
	file.forEachTokenLocationLine([&ostream](TokenLocationLine* l)
	{
		ostream << *l << '\n';
	});
	return ostream;
}

