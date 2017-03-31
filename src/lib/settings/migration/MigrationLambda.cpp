#include "settings/migration/MigrationLambda.h"

MigrationLambda::MigrationLambda(std::function<void(const Migration*, Settings*)> m_lambda)
	: m_lambda(m_lambda)
{
}

void MigrationLambda::apply(Settings* settings) const
{
	m_lambda(this, settings);
}
