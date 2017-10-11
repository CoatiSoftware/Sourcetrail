#include "data/location/LocationType.h"

int locationTypeToInt(LocationType type)
{
	return type;
}

LocationType intToLocationType(int value)
{
	switch (value)
	{
	case LOCATION_TOKEN:
		return LOCATION_TOKEN;
	case LOCATION_SCOPE:
		return LOCATION_SCOPE;
	case LOCATION_QUALIFIER:
		return LOCATION_QUALIFIER;
	case LOCATION_LOCAL_SYMBOL:
		return LOCATION_LOCAL_SYMBOL;
	case LOCATION_ERROR:
		return LOCATION_ERROR; 
	case LOCATION_FULLTEXT_SEARCH:
		return LOCATION_FULLTEXT_SEARCH;
	case LOCATION_SCREEN_SEARCH:
		return LOCATION_SCREEN_SEARCH;
	}
	return LOCATION_TOKEN;
}
