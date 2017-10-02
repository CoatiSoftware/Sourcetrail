#include "project/SourceGroupCxx.h"

#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/CompilationDatabase.h"
#include "clang/Tooling/JSONCompilationDatabase.h"

#include "data/indexer/IndexerCommandCxxManual.h"
#include "data/indexer/IndexerCommandCxxCdb.h"
#include "settings/ApplicationSettings.h"
#include "utility/file/FileManager.h"
#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageStatus.h"
#include "utility/utility.h"

#include "Application.h"

SourceGroupCxx::SourceGroupCxx()
{
}

SourceGroupCxx::~SourceGroupCxx()
{
}

std::shared_ptr<SourceGroupSettings> SourceGroupCxx::getSourceGroupSettings()
{
	return getSourceGroupSettingsCxx();
}
