#include "settings/migration/MigrationMoveKey.h"

MigrationMoveKey::MigrationMoveKey(const std::string& oldKey, const std::string& newKey)
	: m_oldKey(oldKey)
	, m_newKey(newKey)
{
}

void MigrationMoveKey::apply(Settings* settings) const
{
	if (!isValueDefinedInSettings(settings, m_newKey))
	{
		setValuesInSettings<std::string>(
			settings,
			m_newKey,
			getValuesFromSettings<std::string>(settings, m_oldKey, std::vector<std::string>())
		);
		removeValuesInSettings(settings, m_oldKey);
	}
}
