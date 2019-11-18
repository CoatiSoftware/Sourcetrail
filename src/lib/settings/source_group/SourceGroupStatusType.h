#ifndef SOURCE_GROUP_STATUS_TYPE_H
#define SOURCE_GROUP_STATUS_TYPE_H

#include <string>

enum SourceGroupStatusType
{
	SOURCE_GROUP_STATUS_ENABLED,
	SOURCE_GROUP_STATUS_DISABLED
};

std::string sourceGroupStatusTypeToString(SourceGroupStatusType v);
SourceGroupStatusType stringToSourceGroupStatusType(std::string v);

#endif	  // SOURCE_GROUP_STATUS_TYPE_H
