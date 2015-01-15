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
	virtual TokenLocationFile getTokenLocationsForFile(const std::string& filePath) const;
	virtual TokenLocationFile getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const;

	virtual TokenLocationCollection getErrorTokenLocations(std::vector<std::string>* errorMessages) const;

private:
	LocationAccess* m_subject;
};

#endif // LOCATION_ACCESS_PROXY_H
