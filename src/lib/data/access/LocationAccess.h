#ifndef LOCATION_ACCESS_H
#define LOCATION_ACCESS_H

#include <string>
#include <vector>

#include "utility/types.h"

class TokenLocationCollection;
class TokenLocationFile;

class LocationAccess
{
public:
	virtual ~LocationAccess();
	virtual TokenLocationCollection getTokenLocationsForLocationIds(const std::vector<Id>& locationIds) const = 0;
	virtual TokenLocationFile getTokenLocationsForLinesInFile(
		const std::string& fileName, unsigned int firstLineNumber, unsigned int lastLineNumber
	) const = 0;
};


#endif // LOCATION_ACCESS_H
