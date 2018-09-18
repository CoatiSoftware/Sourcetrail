#include "SettingsMigrationLambda.h"

SettingsMigrationLambda::SettingsMigrationLambda(std::function<void(const SettingsMigration*, Settings*)> m_lambda)
	: m_lambda(m_lambda)
{
}

SettingsMigrationLambda::~SettingsMigrationLambda()
{
}

void SettingsMigrationLambda::apply(Settings* migratable) const
{
	m_lambda(this, migratable);
}
