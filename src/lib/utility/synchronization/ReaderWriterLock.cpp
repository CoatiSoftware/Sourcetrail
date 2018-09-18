#include "ReaderWriterLock.h"

ReaderWriterLock::ReaderWriterLock()
	: m_readerCount(0)
	, m_allowedReaders(1)
	, m_allowedWriters(1)
{
}

ReaderWriterLock::ReaderWriterLock(const ReaderWriterLock &lock)
	: m_readerCount(0)
	, m_allowedReaders(1)
	, m_allowedWriters(1)
{
}

ReaderWriterLock::~ReaderWriterLock()
{
}

void ReaderWriterLock::readerLock()
{
	m_allowedReaders.wait();
	m_allowedReaders.post();

	std::lock_guard<std::mutex> lock(m_readerCountMutex);
	m_readerCount++;
	if (m_readerCount == 1)
	{
		m_allowedWriters.wait();
	}
}

void ReaderWriterLock::readerUnlock()
{
	std::lock_guard<std::mutex> lock(m_readerCountMutex);
	m_readerCount--;
	if (m_readerCount == 0)
	{
		m_allowedWriters.post();
	}
}

void ReaderWriterLock::writerLock()
{
	m_allowedReaders.wait();
	m_allowedWriters.wait();
}

void ReaderWriterLock::writerUnlock()
{
	m_allowedWriters.post();
	m_allowedReaders.post();
}
