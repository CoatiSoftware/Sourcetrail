#include "data/access/LocationAccessProxy.h"

#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"
#include "utility/logging/logging.h"

LocationAccessProxy::LocationAccessProxy()
	: m_subject(nullptr)
{
}

LocationAccessProxy::~LocationAccessProxy()
{
}

bool LocationAccessProxy::hasSubject() const
{
	if (m_subject)
	{
		return true;
	}

	LOG_ERROR("LocationAccessProxy has no subject.");
	return false;
}

void LocationAccessProxy::setSubject(LocationAccess* subject)
{
	m_subject = subject;
}

TokenLocationCollection LocationAccessProxy::getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForLocationIds(locationIds);
	}

	return TokenLocationCollection();
}

TokenLocationFile LocationAccessProxy::getTokenLocationsForFile(const std::string& filePath) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForFile(filePath);
	}

	return TokenLocationFile("");
}

TokenLocationFile LocationAccessProxy::getTokenLocationsForLinesInFile(
	const std::string& filePath, uint firstLineNumber, uint lastLineNumber
) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForLinesInFile(filePath, firstLineNumber, lastLineNumber);
	}

	return TokenLocationFile("");
}

TokenLocationCollection LocationAccessProxy::getErrorTokenLocations(std::vector<std::string>* errorMessages) const
{
	if (hasSubject())
	{
		return m_subject->getErrorTokenLocations(errorMessages);
	}

	return TokenLocationCollection();
}
