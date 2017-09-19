#ifndef LOCATION_TYPE_H
#define LOCATION_TYPE_H

enum LocationType
{
	LOCATION_TOKEN,
	LOCATION_SCOPE,
	LOCATION_LOCAL_SYMBOL,
	LOCATION_ERROR,
	LOCATION_FULLTEXT_SEARCH,
	LOCATION_SCREEN_SEARCH
};

int locationTypeToInt(LocationType type);
LocationType intToLocationType(int value);

#endif // LOCATION_TYPE_H
