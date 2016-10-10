#ifndef READER_WRITER_LOCK_H
#define READER_WRITER_LOCK_H

#include <mutex>
#include "Semaphore.h"

class ReaderWriterLock
{
public:
	ReaderWriterLock();

private:
	ReaderWriterLock(const ReaderWriterLock &lock);

public:
	~ReaderWriterLock();

	void readerLock();
	void readerUnlock();
	void writerLock();
	void writerUnlock();

private:
	int m_readerCount;
	std::mutex m_readerCountMutex;
	Semaphore m_allowedReaders, m_allowedWriters;
};

#endif // READER_WRITER_LOCK_H

