#ifndef LOCATION_TYPE_H
#define LOCATION_TYPE_H

enum LocationType
{
	LOCATION_TOKEN,
	LOCATION_SCOPE,
	LOCATION_LOCAL_SYMBOL,
	LOCATION_FULLTEXTSEARCH_MATCH
};

int locationTypeToInt(LocationType type);
LocationType intToLocationType(int value);

#endif // LOCATION_TYPE_H
