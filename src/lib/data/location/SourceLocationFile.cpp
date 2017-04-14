#include "data/location/SourceLocationFile.h"

SourceLocationFile::SourceLocationFile(const FilePath& filePath, bool isWhole, bool isComplete)
	: m_filePath(filePath)
	, m_isWhole(isWhole)
	, m_isComplete(isComplete)
{
}

SourceLocationFile::~SourceLocationFile()
{
}

const FilePath& SourceLocationFile::getFilePath() const
{
	return m_filePath;
}

void SourceLocationFile::setIsWhole(bool isWhole)
{
	m_isWhole = isWhole;
}

bool SourceLocationFile::isWhole() const
{
	return m_isWhole;
}

void SourceLocationFile::setIsComplete(bool isComplete)
{
	m_isComplete = isComplete;
}

bool SourceLocationFile::isComplete() const
{
	return m_isComplete;
}

const std::multiset<std::shared_ptr<SourceLocation>, SourceLocationFile::LocationComp>& SourceLocationFile::getSourceLocations() const
{
	return m_locations;
}

size_t SourceLocationFile::getSourceLocationCount() const
{
	return m_locationIndex.size();
}

size_t SourceLocationFile::getUnscopedStartLocationCount() const
{
	size_t count = 0;
	for (std::shared_ptr<SourceLocation> location : m_locations)
	{
		if (location->isStartLocation() && !location->isScopeLocation())
		{
			count++;
		}
	}
	return count;
}

SourceLocation* SourceLocationFile::addSourceLocation(
	LocationType type, Id locationId, std::vector<Id> tokenIds,
	size_t startLineNumber, size_t startColumnNumber,
	size_t endLineNumber, size_t endColumnNumber)
{
	std::shared_ptr<SourceLocation> start =
		std::make_shared<SourceLocation>(this, type, locationId, tokenIds, startLineNumber, startColumnNumber, true);
	std::shared_ptr<SourceLocation> end = std::make_shared<SourceLocation>(start.get(), endLineNumber, endColumnNumber);

	m_locations.insert(start);
	m_locations.insert(end);

	m_locationIndex.emplace(start->getLocationId(), start.get());

	return start.get();
}

SourceLocation* SourceLocationFile::addSourceLocationCopy(const SourceLocation* location)
{
	// Check whether this location was already added or if the other SourceLocation was added.
	SourceLocation* oldLocation = getSourceLocationById(location->getLocationId());
	if (oldLocation)
	{
		if (oldLocation->isStartLocation() == location->isStartLocation())
		{
			return oldLocation;
		}

		const SourceLocation* otherOldLocation = oldLocation->getOtherLocation();
		if (otherOldLocation && otherOldLocation->isStartLocation() == location->isStartLocation())
		{
			return const_cast<SourceLocation*>(otherOldLocation);
		}
	}

	std::shared_ptr<SourceLocation> copy = std::make_shared<SourceLocation>(location, this);
	m_locations.insert(copy);
	m_locationIndex.emplace(copy->getLocationId(), copy.get());

	// If the old location was added before, then link them with each other.
	if (oldLocation)
	{
		oldLocation->setOtherLocation(copy.get());
		copy->setOtherLocation(oldLocation);
	}

	return copy.get();
}

SourceLocation* SourceLocationFile::getSourceLocationById(Id locationId) const
{
	std::map<Id, SourceLocation*>::const_iterator it = m_locationIndex.find(locationId);

	if (it != m_locationIndex.end())
	{
		return it->second;
	}

	return nullptr;
}

void SourceLocationFile::forEachSourceLocation(std::function<void(SourceLocation*)> func) const
{
	for (std::shared_ptr<SourceLocation> location : m_locations)
	{
		func(location.get());
	}
}

void SourceLocationFile::forEachStartSourceLocation(std::function<void(SourceLocation*)> func) const
{
	for (std::shared_ptr<SourceLocation> location : m_locations)
	{
		if (location->isStartLocation())
		{
			func(location.get());
		}
	}
}

void SourceLocationFile::forEachEndSourceLocation(std::function<void(SourceLocation*)> func) const
{
	for (std::shared_ptr<SourceLocation> location : m_locations)
	{
		if (location->isEndLocation())
		{
			func(location.get());
		}
	}
}

std::shared_ptr<SourceLocationFile> SourceLocationFile::getFilteredByLines(size_t firstLineNumber, size_t lastLineNumber) const
{
	std::shared_ptr<SourceLocationFile> ret = std::make_shared<SourceLocationFile>(getFilePath(), false, isComplete());

	for (std::shared_ptr<SourceLocation> location : m_locations)
	{
		if (location->getLineNumber() >= firstLineNumber && location->getLineNumber() <= lastLineNumber)
		{
			ret->addSourceLocationCopy(location.get());
		}
	}

	return ret;
}

std::shared_ptr<SourceLocationFile> SourceLocationFile::getFilteredByType(LocationType type) const
{
	std::shared_ptr<SourceLocationFile> ret = std::make_shared<SourceLocationFile>(getFilePath(), false, isComplete());

	for (std::shared_ptr<SourceLocation> location : m_locations)
	{
		if (location->getType() == type)
		{
			ret->addSourceLocationCopy(location.get());
		}
	}

	return ret;
}

std::ostream& operator<<(std::ostream& ostream, const SourceLocationFile& file)
{
	ostream << "file \"" << file.getFilePath().str() << "\"";

	size_t line = 0;
	file.forEachSourceLocation(
		[&ostream, &line](SourceLocation* location)
		{
			if (location->getLineNumber() != line)
			{
				while (line < location->getLineNumber())
				{
					if (!line)
					{
						line = location->getLineNumber();
					}
					else
					{
						line++;
					}

					ostream << '\n' << line;
				}

				ostream << ":  ";
			}

			ostream << *location;
		}
	);

	ostream << '\n';
	return ostream;
}
