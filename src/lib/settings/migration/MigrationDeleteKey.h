#ifndef MIGRATION_DELETE_KEY_H
#define MIGRATION_DELETE_KEY_H

#include <string>

#include "settings/migration/Migration.h"

class MigrationDeleteKey: public Migration
{
public:
	MigrationDeleteKey(const std::string& key);
	virtual void apply(Settings* settings) const;

private:
	const std::string m_key;
};

#endif // MIGRATION_DELETE_KEY_H
