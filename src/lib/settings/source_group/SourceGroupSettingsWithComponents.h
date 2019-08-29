#ifndef SOURCE_GROUP_SETTINGS_WITH_COMPONENTS_H
#define SOURCE_GROUP_SETTINGS_WITH_COMPONENTS_H

#include "SourceGroupSettings.h"

template<typename... ComponentTypes>
class SourceGroupSettingsWithComponents
	: public SourceGroupSettings
	, public ComponentTypes...
{
public:
	SourceGroupSettingsWithComponents(SourceGroupType type, const std::string& id, const ProjectSettings* projectSettings)
		: SourceGroupSettings(type, id, projectSettings)
	{
	}

	virtual ~SourceGroupSettingsWithComponents() = default;

	void loadSettings(const ConfigManager* config) override
	{
		const std::string key = s_keyPrefix + getId();

		SourceGroupSettings::load(config, key);

		using expand_type = int[];
		expand_type a{ 0, ((ComponentTypes::load(config, key)), void(), 0)... };
	}

	void saveSettings(ConfigManager* config) override
	{
		const std::string key = s_keyPrefix + getId();

		SourceGroupSettings::save(config, key);

		using expand_type = int[];
		expand_type a{ 0, ((ComponentTypes::save(config, key)), void(), 0)... };
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

		using expand_type = int[];
		expand_type a{ false, ComponentTypes::equals(other)... };

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
};

#endif // SOURCE_GROUP_SETTINGS_WITH_COMPONENTS_H
