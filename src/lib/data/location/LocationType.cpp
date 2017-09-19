#include "data/location/LocationType.h"

int locationTypeToInt(LocationType type)
{
	switch (type)
	{
	case LOCATION_TOKEN:
		return 0;
	case LOCATION_SCOPE:
		return 1;
	case LOCATION_LOCAL_SYMBOL:
		return 2;
	case LOCATION_ERROR:
		return 3;
	case LOCATION_FULLTEXT_SEARCH:
		return 4;
	case LOCATION_SCREEN_SEARCH:
		return 5;
	}
}

LocationType intToLocationType(int value)
{
	switch (value)
	{
	case 0:
		return LOCATION_TOKEN;
	case 1:
		return LOCATION_SCOPE;
	case 2:
		return LOCATION_LOCAL_SYMBOL;
	case 3:
		return LOCATION_ERROR;
	case 4:
		return LOCATION_FULLTEXT_SEARCH;
	case 5:
		return LOCATION_SCREEN_SEARCH;
	}
	return LOCATION_TOKEN;
}
