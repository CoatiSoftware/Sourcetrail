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

TokenLocationCollection LocationAccessProxy::getTokenLocationsForTokenId(Id id) const
{
	if (hasSubject())
	{
		return m_subject->getTokenLocationsForTokenId(id);
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
