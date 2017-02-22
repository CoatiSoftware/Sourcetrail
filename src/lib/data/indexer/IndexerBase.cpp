#include "data/indexer/IndexerBase.h"

IndexerBase::IndexerBase()
	: m_interrupted(false)
{
}

IndexerBase::~IndexerBase()
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
