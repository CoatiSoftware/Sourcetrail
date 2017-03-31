#ifndef MIGRATION_MOVE_KEY_H
#define MIGRATION_MOVE_KEY_H

#include "settings/migration/Migration.h"

class MigrationMoveKey: public Migration
{
public:
	MigrationMoveKey(const std::string& oldKey, const std::string& newKey);
	virtual void apply(Settings* settings) const;

private:
	const std::string m_oldKey;
	const std::string m_newKey;
};

#endif // MIGRATION_MOVE_KEY_H
