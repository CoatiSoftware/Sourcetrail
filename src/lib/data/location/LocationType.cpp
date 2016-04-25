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
	}
	return LOCATION_TOKEN;
}
