#ifndef SOURCE_GROUP_SETTINGS_WITH_COMPONENTS_H
#define SOURCE_GROUP_SETTINGS_WITH_COMPONENTS_H

#include "SourceGroupSettings.h"

template <typename... ComponentTypes>
class SourceGroupSettingsWithComponents
	: public SourceGroupSettings
	, public ComponentTypes...
{
public:
	SourceGroupSettingsWithComponents(
		SourceGroupType type, const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettings(type, id, projectSettings)
	{
	}

	virtual ~SourceGroupSettingsWithComponents() = default;

	void loadSettings(const ConfigManager* config) override
	{
		const std::string key = s_keyPrefix + getId();

		SourceGroupSettings::load(config, key);

		using expand_type = bool[];
		expand_type a {false, loadHelper<ComponentTypes>(config, key)...};
	}

	void saveSettings(ConfigManager* config) override
	{
		const std::string key = s_keyPrefix + getId();

		SourceGroupSettings::save(config, key);

		using expand_type = bool[];
		expand_type a {false, saveHelper<ComponentTypes>(config, key)...};
	}

	bool equalsSettings(const SourceGroupSettingsBase* other) override
	{
		if (!SourceGroupSettings::equals(other))
		{
			return false;
		}

		const SourceGroupSettingsWithComponents<ComponentTypes...>* otherPtr =
			dynamic_cast<const SourceGroupSettingsWithComponents<ComponentTypes...>*>(other);
		if (getComponentCount() != otherPtr->getComponentCount())
		{
			return false;
		}

		using expand_type = bool[];
		expand_type a {false, equalsHelper<ComponentTypes>(other)...};

		bool r = true;
		for (size_t i = 1; i <= getComponentCount(); ++i)
		{
			r &= a[i];
		}

		return r;
	}

private:
	size_t getComponentCount() const
	{
		return sizeof...(ComponentTypes);
	}

	template <typename T>
	bool loadHelper(const ConfigManager* config, const std::string& key)
	{
		T::load(config, key);
		return true;
	}

	template <typename T>
	bool saveHelper(ConfigManager* config, const std::string& key)
	{
		T::save(config, key);
		return true;
	}

	template <typename T>
	bool equalsHelper(const SourceGroupSettingsBase* other)
	{
		return T::equals(other);
	}
};

#endif	  // SOURCE_GROUP_SETTINGS_WITH_COMPONENTS_H
