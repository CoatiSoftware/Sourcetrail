#include "IndexerBase.h"

IndexerBase::IndexerBase()
	: m_interrupted(false)
{
}

void IndexerBase::interrupt()
{
	m_interrupted = true;
}

bool IndexerBase::interrupted() const
{
	return m_interrupted;
}
