#include "data/location/SourceLocationCollection.h"

#include "data/location/SourceLocationFile.h"
#include "utility/logging/logging.h"

SourceLocationCollection::SourceLocationCollection()
{
}

SourceLocationCollection::~SourceLocationCollection()
{
}

const std::map<FilePath, std::shared_ptr<SourceLocationFile>>& SourceLocationCollection::getSourceLocationFiles() const
{
	return m_files;
}

size_t SourceLocationCollection::getSourceLocationCount() const
{
	size_t count = 0;
	for (auto& p : m_files)
	{
		count += p.second->getSourceLocationCount();
	}
	return count;
}

size_t SourceLocationCollection::getSourceLocationFileCount() const
{
	return m_files.size();
}

std::shared_ptr<SourceLocationFile> SourceLocationCollection::getSourceLocationFileByPath(const FilePath& filePath) const
{
	std::map<FilePath, std::shared_ptr<SourceLocationFile>>::const_iterator it = m_files.find(filePath);
	if (it != m_files.end())
	{
		return it->second;
	}

	return nullptr;
}

SourceLocation* SourceLocationCollection::getSourceLocationById(Id locationId) const
{
	for (auto& p : m_files)
	{
		SourceLocation* location = p.second->getSourceLocationById(locationId);
		if (location)
		{
			return location;
		}
	}

	return nullptr;
}

SourceLocation* SourceLocationCollection::addSourceLocation(
	LocationType type, Id locationId, std::vector<Id> tokenIds, const FilePath& filePath,
	size_t startLineNumber, size_t startColumnNumber,
	size_t endLineNumber, size_t endColumnNumber)
{
	if (startLineNumber > endLineNumber || (startLineNumber == endLineNumber && startColumnNumber > endColumnNumber))
	{
		LOG_ERROR(L"SourceLocation has wrong boundaries: " + filePath.wstr() + L" " +
			std::to_wstring(startLineNumber) + L":" + std::to_wstring(startColumnNumber) + L" " +
			std::to_wstring(endLineNumber) + L":" + std::to_wstring(endColumnNumber));
		return nullptr;
	}

	SourceLocationFile* file = createSourceLocationFile(filePath);
	if (file->isWhole())
	{
		return nullptr;
	}

	return file->addSourceLocation(type, locationId, tokenIds, startLineNumber, startColumnNumber, endLineNumber, endColumnNumber);
}

SourceLocation* SourceLocationCollection::addSourceLocationCopy(SourceLocation* location)
{
	return createSourceLocationFile(location->getFilePath())->addSourceLocationCopy(location);
}

void SourceLocationCollection::addSourceLocationFile(std::shared_ptr<SourceLocationFile> file)
{
	m_files.emplace(file->getFilePath(), file);
}

void SourceLocationCollection::forEachSourceLocationFile(
	std::function<void(std::shared_ptr<SourceLocationFile>)> func) const
{
	for (auto& p : m_files)
	{
		func(p.second);
	}
}

void SourceLocationCollection::forEachSourceLocation(std::function<void(SourceLocation*)> func) const
{
	for (auto& p : m_files)
	{
		p.second->forEachSourceLocation(func);
	}
}

SourceLocationFile* SourceLocationCollection::createSourceLocationFile(const FilePath& filePath)
{
	SourceLocationFile* file = getSourceLocationFileByPath(filePath).get();
	if (file)
	{
		return file;
	}

	std::shared_ptr<SourceLocationFile> filePtr = std::make_shared<SourceLocationFile>(filePath, false, false);
	m_files.emplace(filePath, filePtr);
	return filePtr.get();
}

std::wostream& operator<<(std::wostream& ostream, const SourceLocationCollection& base)
{
	ostream << L"Locations:\n";
	base.forEachSourceLocationFile([&ostream](std::shared_ptr<SourceLocationFile> f)
	{
		ostream << *(f.get());
	});
	return ostream;
}
