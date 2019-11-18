#ifndef MIGRATION_H
#define MIGRATION_H

template <typename MigratableType>
class Migration
{
public:
	virtual ~Migration();
	virtual void apply(MigratableType* migratable) const = 0;
};

template <typename MigratableType>
Migration<MigratableType>::~Migration()
{
}

#endif	  // MIGRATION_H
