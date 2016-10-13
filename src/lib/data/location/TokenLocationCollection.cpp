#include "data/location/TokenLocationCollection.h"

#include <algorithm>

#include "utility/file/FileSystem.h"
#include "utility/logging/logging.h"

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"

TokenLocationCollection::TokenLocationCollection()
{
}

TokenLocationCollection::~TokenLocationCollection()
{
}

const TokenLocationCollection::TokenLocationFileMapType& TokenLocationCollection::getTokenLocationFiles() const
{
	return m_files;
}

const std::map<Id, TokenLocation*>& TokenLocationCollection::getTokenLocations() const
{
	return m_locations;
}

std::shared_ptr<TokenLocationFile> TokenLocationCollection::getTokenLocationFileByPath(const FilePath& filePath) const
{
	std::map<FilePath, std::shared_ptr<TokenLocationFile>>::const_iterator it = m_files.find(filePath);
	if (it != m_files.end())
	{
		return it->second;
	}

	return nullptr;
}

size_t TokenLocationCollection::getTokenLocationFileCount() const
{
	return m_files.size();
}

size_t TokenLocationCollection::getTokenLocationLineCount() const
{
	size_t count = 0;

	for (const TokenLocationFilePairType& file : m_files)
	{
		count += file.second->getTokenLocationLineCount();
	}

	return count;
}

size_t TokenLocationCollection::getTokenLocationCount() const
{
	return m_locations.size();
}

TokenLocation* TokenLocationCollection::addTokenLocation(
	Id locationId, Id tokenId, const FilePath& filePath,
	unsigned int startLineNumber, unsigned int startColumnNumber,
	unsigned int endLineNumber, unsigned int endColumnNumber)
{
	if (startLineNumber > endLineNumber || (startLineNumber == endLineNumber && startColumnNumber > endColumnNumber))
	{
		LOG_ERROR_STREAM(<< "TokenLocation has wrong boundaries: "<< filePath.str() << " "
			<< startLineNumber << ":" << startColumnNumber << " "
			<< endLineNumber << ":" << endColumnNumber);
		return nullptr;
	}

	TokenLocation* location = findTokenLocationById(locationId);
	if (location)
	{
		return location;
	}

	TokenLocationFile* file = createTokenLocationFile(filePath);
	location = file->addTokenLocation(
		locationId, tokenId, startLineNumber, startColumnNumber, endLineNumber, endColumnNumber);

	m_locations.emplace(location->getId(), location);
	return location;
}

void TokenLocationCollection::removeTokenLocation(TokenLocation* location)
{
	if (!location || !findTokenLocationById(location->getId()))
	{
		LOG_ERROR("TokenLocation is not part of this TokenLocationCollection.");
		return;
	}

	m_locations.erase(location->getId());

	TokenLocationFile* file = location->getTokenLocationFile();
	file->removeTokenLocation(location);

	if (!file->getTokenLocationLineCount())
	{
		m_files.erase(file->getFilePath());
	}
}

TokenLocationFile* TokenLocationCollection::addTokenLocationFile(std::shared_ptr<TokenLocationFile> locationFile)
{
	TokenLocationFile* file = findTokenLocationFileByPath(locationFile->getFilePath());
	if (file)
	{
		LOG_ERROR("TokenLocationFile with same path already exists.");
		return file;
	}

	m_files.emplace(locationFile->getFilePath(), locationFile);
	locationFile->forEachTokenLocation(
		[this, &file](TokenLocation* tokenLocation) -> void
		{
			m_locations.emplace(tokenLocation->getId(), tokenLocation);
		}
	);
	return locationFile.get();
}

void TokenLocationCollection::removeTokenLocationFile(TokenLocationFile* file)
{
	file->forEachTokenLocation(
		[&](TokenLocation* location)
		{
			m_locations.erase(location->getId());
		}
	);

	m_files.erase(file->getFilePath());
}

TokenLocation* TokenLocationCollection::findTokenLocationById(Id id) const
{
	std::map<Id, TokenLocation*>::const_iterator it = m_locations.find(id);

	if (it != m_locations.end())
	{
		return it->second;
	}

	return nullptr;
}

TokenLocationFile* TokenLocationCollection::findTokenLocationFileByPath(const FilePath& filePath) const
{
	return getTokenLocationFileByPath(filePath).get();
}

void TokenLocationCollection::forEachTokenLocationFile(
	std::function<void(std::shared_ptr<TokenLocationFile>)> func) const
{
	for (const TokenLocationFilePairType& file : m_files)
	{
		func(file.second);
	}
}

void TokenLocationCollection::forEachTokenLocationLine(std::function<void(TokenLocationLine*)> func) const
{
	for (const TokenLocationFilePairType& file : m_files)
	{
		file.second->forEachTokenLocationLine(func);
	}
}

void TokenLocationCollection::forEachTokenLocation(std::function<void(TokenLocation*)> func) const
{
	for (const TokenLocationFilePairType& file : m_files)
	{
		file.second->forEachTokenLocation(func);
	}
}

TokenLocationFile* TokenLocationCollection::addTokenLocationFileAsPlainCopy(const TokenLocationFile* locationFile)
{
	TokenLocationFile* file = createTokenLocationFile(locationFile->getFilePath());
	locationFile->forEachTokenLocation(
		[this, &file](TokenLocation* tokenLocation) -> void
		{
			TokenLocation* copy = file->addTokenLocationAsPlainCopy(tokenLocation);
			m_locations.emplace(copy->getId(), copy);
		}
	);
	return file;
}

TokenLocation* TokenLocationCollection::addTokenLocationAsPlainCopy(const TokenLocation* location)
{
	const FilePath& filePath = location->getTokenLocationLine()->getTokenLocationFile()->getFilePath();
	TokenLocationFile* file = createTokenLocationFile(filePath);
	TokenLocation* copy = file->addTokenLocationAsPlainCopy(location);

	m_locations.emplace(copy->getId(), copy);
	return copy;
}

void TokenLocationCollection::clear()
{
	m_locations.clear();
	m_files.clear();
}

TokenLocationFile* TokenLocationCollection::createTokenLocationFile(const FilePath& filePath)
{
	TokenLocationFile* file = findTokenLocationFileByPath(filePath);

	if (file)
	{
		return file;
	}

	std::shared_ptr<TokenLocationFile> filePtr = std::make_shared<TokenLocationFile>(filePath);
	m_files.emplace(filePath, filePtr);
	return filePtr.get();
}

std::ostream& operator<<(std::ostream& ostream, const TokenLocationCollection& base)
{
	ostream << "Locations:\n";
	base.forEachTokenLocationFile([&ostream](std::shared_ptr<TokenLocationFile> f)
	{
		ostream << *(f.get());
	});
	return ostream;
}
