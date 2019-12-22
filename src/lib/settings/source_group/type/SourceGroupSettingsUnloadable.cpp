#include "SourceGroupSettingsUnloadable.h"

#include "ConfigManager.h"
#include "utility.h"

SourceGroupSettingsUnloadable::SourceGroupSettingsUnloadable(
	const std::string& id, const ProjectSettings* projectSettings)
	: SourceGroupSettings(SOURCE_GROUP_UNKNOWN, id, projectSettings)
{
}

std::string SourceGroupSettingsUnloadable::getTypeString()
{
	return m_typeString;
}

std::shared_ptr<SourceGroupSettings> SourceGroupSettingsUnloadable::createCopy() const
{
	return std::make_shared<SourceGroupSettingsUnloadable>(*this);
}

void SourceGroupSettingsUnloadable::loadSettings(const ConfigManager* config)
{
	const std::string key = s_keyPrefix + getId();

	SourceGroupSettings::load(config, key);
	setStatus(SOURCE_GROUP_STATUS_DISABLED);

	m_typeString = config->getValueOrDefault<std::string>(key + "/type", "");

	m_content.clear();

	std::vector<std::string> unprocessedKeys = {key};

	while (!unprocessedKeys.empty())
	{
		const std::string unprocessedKey = unprocessedKeys.back();
		unprocessedKeys.pop_back();
		for (const std::string& memberKey: config->getSublevelKeys(unprocessedKey))
		{
			const std::vector<std::string> values = config->getValuesOrDefaults<std::string>(
				memberKey, {});
			if (!values.empty())
			{
				m_content[memberKey] = values;
			}
			else
			{
				unprocessedKeys.push_back(memberKey);
			}
		}
	}
}

void SourceGroupSettingsUnloadable::saveSettings(ConfigManager* config)
{
	for (auto it: m_content)
	{
		config->setValues(it.first, it.second);
	}
}

bool SourceGroupSettingsUnloadable::equalsSettings(const SourceGroupSettingsBase* other)
{
	if (!SourceGroupSettings::equals(other))
	{
		return false;
	}

	if (const SourceGroupSettingsUnloadable* otherUnloadable =
			dynamic_cast<const SourceGroupSettingsUnloadable*>(other))
	{
		for (auto it: m_content)
		{
			auto otherIt = otherUnloadable->m_content.find(it.first);
			if (otherIt == otherUnloadable->m_content.end() ||
				!utility::isPermutation(it.second, otherIt->second))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}
