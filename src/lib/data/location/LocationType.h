#ifndef LOCATION_TYPE_H
#define LOCATION_TYPE_H

enum LocationType
{
	LOCATION_TOKEN = 0,
	LOCATION_SCOPE = 1,
	LOCATION_QUALIFIER = 2,
	LOCATION_LOCAL_SYMBOL = 3,
	LOCATION_ERROR = 4,
	LOCATION_FULLTEXT_SEARCH = 5,
	LOCATION_SCREEN_SEARCH = 6
};

int locationTypeToInt(LocationType type);
LocationType intToLocationType(int value);

#endif // LOCATION_TYPE_H
