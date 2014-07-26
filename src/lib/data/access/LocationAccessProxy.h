#ifndef LOCATION_ACCESS_PROXY_H
#define LOCATION_ACCESS_PROXY_H

#include "data/access/LocationAccess.h"

class LocationAccessProxy: public LocationAccess
{
public:
	LocationAccessProxy();
	virtual ~LocationAccessProxy();

	bool hasSubject() const;
	void setSubject(LocationAccess* subject);

	// LocationAccess implementation
	virtual TokenLocationCollection getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const;
	virtual TokenLocationFile getTokenLocationsForLinesInFile(
		const std::string& fileName, unsigned int firstLineNumber, unsigned int lastLineNumber
	) const;

private:
	LocationAccess* m_subject;
};

#endif // LOCATION_ACCESS_PROXY_H
