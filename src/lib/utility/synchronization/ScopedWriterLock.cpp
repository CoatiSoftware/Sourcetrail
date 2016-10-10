#include "utility/synchronization/ScopedWriterLock.h"

ScopedWriterLock::ScopedWriterLock(ReaderWriterLock& lock)
	: m_lock(lock)
{
	m_lock.writerLock();
}

ScopedWriterLock::~ScopedWriterLock()
{
	m_lock.writerUnlock();
}
