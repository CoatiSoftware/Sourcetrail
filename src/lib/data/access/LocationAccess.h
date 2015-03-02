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
	virtual TokenLocationCollection getTokenLocationsForTokenIds(const std::vector<Id>& tokenIds) const = 0;
	virtual TokenLocationFile getTokenLocationsForFile(const std::string& filePath) const = 0;
	virtual TokenLocationFile getTokenLocationsForLinesInFile(
		const std::string& filePath, uint firstLineNumber, uint lastLineNumber
	) const = 0;

	virtual TokenLocationCollection getErrorTokenLocations(std::vector<std::string>* errorMessages) const = 0;
};


#endif // LOCATION_ACCESS_H
