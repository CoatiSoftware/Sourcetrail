#include "FullTextSearchIndex.h"
#include <limits>

#include "logging.h"
#include "tracing.h"

void FullTextSearchIndex::addFile(Id fileId, const std::wstring& fileContent)
{
	if (fileContent.empty())
	{
		LOG_ERROR("empty file not added to fulltextsearch index");
	}

	if (static_cast<int>(fileContent.size()) >= std::numeric_limits<int>::max())
	{
		LOG_ERROR("file too big not added to fulltextsearch index");
	}

	FullTextSearchFile fts_file(fileId, SuffixArray(fileContent));

	{
		std::lock_guard<std::mutex> lock(m_filesMutex);
		m_files.push_back(fts_file);
	}
}

std::vector<FullTextSearchResult> FullTextSearchIndex::searchForTerm(const std::wstring& term) const
{
	TRACE();

	std::vector<FullTextSearchResult> ret;
	{
		std::lock_guard<std::mutex> lock(m_filesMutex);
		for (auto& f: m_files)
		{
			FullTextSearchResult hit;
			hit.fileId = f.fileId;
			hit.positions = f.array.searchForTerm(term);
			std::sort(hit.positions.begin(), hit.positions.end());
			if (!hit.positions.empty())
			{
				ret.push_back(hit);
			}
		}
	}

	return ret;
}

size_t FullTextSearchIndex::fileCount() const
{
	std::lock_guard<std::mutex> lock(m_filesMutex);
	return m_files.size();
}

void FullTextSearchIndex::clear()
{
	std::lock_guard<std::mutex> lock(m_filesMutex);
	m_files.clear();
}
