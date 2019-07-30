#ifndef UTILITY_SOURCE_GROUP_CXX_H
#define UTILITY_SOURCE_GROUP_CXX_H

#include <memory>
#include <string>
#include <vector>

class DialogView;
class SourceGroupSettingsCxx;
class Task;

namespace utility
{
	std::shared_ptr<Task> createBuildPchTask(
		const SourceGroupSettingsCxx* settings, std::vector<std::wstring> compilerFlags, std::shared_ptr<DialogView> dialogView);
	std::vector<std::wstring> getIncludePchFlags(const SourceGroupSettingsCxx* settings);
}

#endif // UTILITY_SOURCE_GROUP_CXX_H
