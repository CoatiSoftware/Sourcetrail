#ifndef SQLITE_STORAGE_MIGRATOR_H
#define SQLITE_STORAGE_MIGRATOR_H

#include "Migrator.h"

class SqliteStorage;

typedef Migrator<SqliteStorage> SqliteStorageMigrator;

#endif // SQLITE_STORAGE_MIGRATOR_H
