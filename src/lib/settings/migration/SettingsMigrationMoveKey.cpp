#include "SettingsMigrationMoveKey.h"

SettingsMigrationMoveKey::SettingsMigrationMoveKey(const std::string& oldKey, const std::string& newKey)
	: m_oldKey(oldKey)
	, m_newKey(newKey)
{
}

SettingsMigrationMoveKey::~SettingsMigrationMoveKey()
{
}

void SettingsMigrationMoveKey::apply(Settings* migratable) const
{
	if (!isValueDefinedInSettings(migratable, m_newKey))
	{
		setValuesInSettings<std::string>(
			migratable,
			m_newKey,
			getValuesFromSettings<std::string>(migratable, m_oldKey, std::vector<std::string>())
		);
		removeValuesInSettings(migratable, m_oldKey);
	}
}
