#include "utility/synchronization/ScopedReaderLock.h"

ScopedReaderLock::ScopedReaderLock(ReaderWriterLock& lock)
	: m_lock(lock)
{
	m_lock.readerLock();
}

ScopedReaderLock::~ScopedReaderLock()
{
	m_lock.readerUnlock();
}
