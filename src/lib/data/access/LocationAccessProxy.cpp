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


TokenLocationFile LocationAccessProxy::getTokenLocationsForLinesInFile(
	const std::string& fileName, unsigned int firstLineNumber, unsigned int lastLineNumber
) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForLinesInFile(fileName, firstLineNumber, lastLineNumber);
	}

	return TokenLocationFile("");
}
