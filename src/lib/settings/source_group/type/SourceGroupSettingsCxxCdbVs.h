#ifndef SOURCE_GROUP_SETTINGS_CXX_CDB_VS_H
#define SOURCE_GROUP_SETTINGS_CXX_CDB_VS_H

#include "language_packages.h"

#if BUILD_CXX_LANGUAGE_PACKAGE

#include "SourceGroupSettingsCxxCdb.h"

class SourceGroupSettingsCxxCdbVs
	: public SourceGroupSettingsCxxCdb
{
	using SourceGroupSettingsCxxCdb::SourceGroupSettingsCxxCdb;
};

#endif // BUILD_CXX_LANGUAGE_PACKAGE

#endif // SOURCE_GROUP_SETTINGS_CXX_CDB_VS_H
