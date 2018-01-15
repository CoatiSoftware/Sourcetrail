#include "settings/SourceGroupStatusType.h"

std::string sourceGroupStatusTypeToString(SourceGroupStatusType v)
{
	switch (v)
	{
	case SOURCE_GROUP_STATUS_ENABLED:
		return "enabled";
	case SOURCE_GROUP_STATUS_DISABLED:
		return "disabled";
	case SOURCE_GROUP_STATUS_UNKNOWN:
		break;
	}
	return "unknown";
}

SourceGroupStatusType stringToSourceGroupStatusType(std::string v)
{
	if (v == sourceGroupStatusTypeToString(SOURCE_GROUP_STATUS_ENABLED))
	{
		return SOURCE_GROUP_STATUS_ENABLED;
	}
	else if (v == sourceGroupStatusTypeToString(SOURCE_GROUP_STATUS_DISABLED))
	{
		return SOURCE_GROUP_STATUS_DISABLED;
	}

	return SOURCE_GROUP_STATUS_UNKNOWN;
}
