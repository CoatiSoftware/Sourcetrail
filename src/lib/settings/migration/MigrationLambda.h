#ifndef MIGRATION_LAMBDA_H
#define MIGRATION_LAMBDA_H

#include <functional>

#include "settings/migration/Migration.h"

class MigrationLambda: public Migration
{
public:
	MigrationLambda(std::function<void(const Migration*, Settings*)> m_lambda);
	virtual void apply(Settings* settings) const;

private:
	std::function<void(const Migration*, Settings*)> m_lambda;
};

#endif // MIGRATION_LAMBDA_H
