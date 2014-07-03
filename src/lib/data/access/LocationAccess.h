#ifndef LOCATION_ACCESS_H
#define LOCATION_ACCESS_H

#include <string>

#include "utility/types.h"

class TokenLocationCollection;
class TokenLocationFile;

class LocationAccess
{
public:
	virtual ~LocationAccess();
	virtual TokenLocationCollection getTokenLocationsForTokenId(Id id) const = 0;
	virtual TokenLocationFile getTokenLocationsForLinesInFile(
		const std::string& fileName, unsigned int firstLineNumber, unsigned int lastLineNumber
	) const = 0;
};


#endif // LOCATION_ACCESS_H
