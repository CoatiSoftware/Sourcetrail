#include "settings/SourceGroupStatusType.h"

#include "utility/logging/logging.h"

std::string sourceGroupStatusTypeToString(SourceGroupStatusType v)
{
	switch (v)
	{
	case SOURCE_GROUP_STATUS_ENABLED:
		return "enabled";
	case SOURCE_GROUP_STATUS_DISABLED:
		return "disabled";
	}
	LOG_WARNING("Trying to convert unknown Source Group status type to string, falling back to disabled status.");
	return "disabled"; 
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

	LOG_WARNING("Trying to convert unknown string to Source Group status type, falling back to disabled status.");
	return SOURCE_GROUP_STATUS_DISABLED;
}
