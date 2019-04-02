#include "SourceGroupFactoryModulePython.h"

#include "SourceGroupPythonEmpty.h"
#include "SourceGroupSettingsPythonEmpty.h"

bool SourceGroupFactoryModulePython::supports(SourceGroupType type) const
{
	switch (type)
	{
	case SOURCE_GROUP_PYTHON_EMPTY:
		return true;
	default:
		break;
	}
	return false;
}

std::shared_ptr<SourceGroup> SourceGroupFactoryModulePython::createSourceGroup(std::shared_ptr<SourceGroupSettings> settings) const
{
	std::shared_ptr<SourceGroup> sourceGroup;
	if (std::shared_ptr<SourceGroupSettingsPythonEmpty> pythonSettings = std::dynamic_pointer_cast<SourceGroupSettingsPythonEmpty>(settings))
	{
		sourceGroup = std::shared_ptr<SourceGroup>(new SourceGroupPythonEmpty(pythonSettings));
	}
	return sourceGroup;
}
