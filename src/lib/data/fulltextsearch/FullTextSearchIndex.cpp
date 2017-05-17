#include "data/fulltextsearch/FullTextSearchIndex.h"
#include <limits>

#include "utility/logging/logging.h"
#include "utility/tracing.h"

void FullTextSearchIndex::addFile(Id fileId, const std::string& file)
{
	if( file.empty() )
	{
		LOG_ERROR("empty file not added to fulltextsearch index");
	}

	if ( file.size() >= std::numeric_limits<int>::max() )
	{
		LOG_ERROR("file too big not added to fulltextsearch index");
	}

	FullTextSearchFile fts_file(fileId, SuffixArray(file));
	m_files.push_back(fts_file);
}

std::vector<FullTextSearchResult> FullTextSearchIndex::searchForTerm(const std::string& term) const
{
	TRACE();

	std::vector<FullTextSearchResult> ret;
	FullTextSearchResult hit;
	for (auto f : m_files)
	{
		hit.fileId = f.fileId;
		hit.positions = f.array.searchForTerm(term);
		ret.push_back(hit);
	}
	return ret;
}

size_t FullTextSearchIndex::fileCount() const
{
	return m_files.size();
}

void FullTextSearchIndex::clear()
{
	m_files.clear();
}

