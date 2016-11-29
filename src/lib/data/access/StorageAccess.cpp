#include "data/access/StorageAccess.h"

StorageAccess::~StorageAccess()
{
}

void StorageAccess::setErrorFilter(const ErrorFilter& filter)
{
	m_errorFilter = filter;
}
