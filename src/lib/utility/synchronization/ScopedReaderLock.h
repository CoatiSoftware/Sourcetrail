#ifndef SCOPED_READER_LOCK_H
#define SCOPED_READER_LOCK_H

#include "ReaderWriterLock.h"

class ScopedReaderLock
{
public:
	ScopedReaderLock(ReaderWriterLock& lock);
	~ScopedReaderLock();

private:
	ReaderWriterLock& m_lock;
};

#endif // SCOPED_READER_LOCK_H
