#ifndef SCOPED_WRITER_LOCK_H
#define SCOPED_WRITER_LOCK_H

#include "utility/synchronization/ReaderWriterLock.h"

class ScopedWriterLock
{
public:
	ScopedWriterLock(ReaderWriterLock& lock);
	~ScopedWriterLock();

private:
	ReaderWriterLock& m_lock;
};

#endif // SCOPED_WRITER_LOCK_H
