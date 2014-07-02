#ifndef LOCATION_ACCESS_H
#define LOCATION_ACCESS_H

#include "utility/types.h"

class TokenLocation;

class LocationAccess
{
public:
	virtual ~LocationAccess();
	virtual TokenLocation* getTokenLocation(Id locationId) = 0;

};


#endif // LOCATION_ACCESS_H
