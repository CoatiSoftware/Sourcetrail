#ifndef SOURCE_GROUP_SETTINGS_UNLOADABLE_H
#define SOURCE_GROUP_SETTINGS_UNLOADABLE_H

#include "SourceGroupSettings.h"

class SourceGroupSettingsUnloadable : public SourceGroupSettings
{
public:
	SourceGroupSettingsUnloadable(const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettings(SOURCE_GROUP_UNKNOWN, id, projectSettings)
	{
	}

	std::shared_ptr<SourceGroupSettings> createCopy() const override
	{
		return std::make_shared<SourceGroupSettingsUnloadable>(*this);
	}

	void loadSettings(const ConfigManager* config) override
	{
		const std::string key = s_keyPrefix + getId();

		SourceGroupSettings::load(config, key);
		setStatus(SOURCE_GROUP_STATUS_DISABLED);

		m_content.clear();

		std::vector<std::string> unprocessedKeys = { key };

		while (!unprocessedKeys.empty())
		{
			const std::string unprocessedKey = unprocessedKeys.back();
			unprocessedKeys.pop_back();
			for (const std::string& memberKey : config->getSublevelKeys(unprocessedKey))
			{
				const std::vector<std::string> values = config->getValuesOrDefaults<std::string>(memberKey, {});
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

	void saveSettings(ConfigManager* config) override
	{
		for (auto it : m_content)
		{
			config->setValues(it.first, it.second);
		}
	}

	bool equalsSettings(const SourceGroupSettingsBase* other) override
	{
		if (!SourceGroupSettings::equals(other))
		{
			return false;
		}
		 //compare values

		return true;
	}

private:
	std::map<std::string, std::vector<std::string>> m_content;

};

#endif // SOURCE_GROUP_SETTINGS_UNLOADABLE_H
