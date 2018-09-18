#include "SnippetMerger.h"

#include <algorithm>
#include "ApplicationSettings.h"

SnippetMerger::SnippetMerger(int startRow, int endRow)
	: m_start(startRow)
	, m_end(endRow)
{
}

void SnippetMerger::addChild(std::shared_ptr<SnippetMerger> child)
{
	m_children.push_back(child);
}

std::deque<SnippetMerger::Range> SnippetMerger::merge(std::vector<SnippetMerger::Range> atomicRanges) const
{
	const int snippetExpandRange = ApplicationSettings::getInstance()->getCodeSnippetExpandRange();
	std::deque<Range> merged;
	if (m_children.size() == 0)
	{
		merged.push_back(Range(Border(m_start, false), Border(m_end, false)));
	}
	else
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			std::deque<Range> mergedFromChild = m_children[i]->merge(atomicRanges);
			for (size_t j = 0; j < mergedFromChild.size(); j++)
			{
				merged.push_back(getExpandedRegardingAtomicRanges(mergedFromChild[j], snippetExpandRange, atomicRanges));
			}
		}
		std::sort(merged.begin(), merged.end(),
			[](const Range& a, const Range& b)
			{
				return a.start.row < b.start.row;
			}
		);

		// merge children
		const int snippetMergeRange = 2 * snippetExpandRange + 1;	// +1 since snippets that end/start with consequtive 
																	// lines should be merged as well.
		merged = Range::mergeAdjacent(merged, snippetMergeRange);

		// snap to own borders
		const int snippetSnapRange = ApplicationSettings::getInstance()->getCodeSnippetSnapRange();
		if ((m_start < merged.front().start.row) && 
			(merged.front().start.row <= m_start + snippetSnapRange))
		{
			merged.front().start.row = m_start;
			merged.front().start.strong = false;
		}
		if ((m_end - snippetSnapRange <= merged.back().end.row) && 
			(merged.back().end.row < m_end))
		{
			merged.back().end.row = m_end;
			merged.back().end.strong = false;
		}
	}
	return merged;
}

SnippetMerger::Range SnippetMerger::getExpandedRegardingAtomicRanges(
	Range range, const int snippetExpandRange, const std::vector<Range>& atomicRanges
) const
{
	const int rangeStartThreshold = range.start.row - snippetExpandRange;
	const int rangeEndThreshold = range.end.row + snippetExpandRange;
	for (size_t i = 0; i < atomicRanges.size(); i++)
	{
		if ((!range.start.strong) &&
			(atomicRanges[i].end.row >= rangeStartThreshold) &&
			(atomicRanges[i].start.row < rangeStartThreshold))
		{
			range.start.row = std::min(range.start.row, atomicRanges[i].start.row);
			range.start.strong = true;
		}
		if ((!range.end.strong) &&
			(atomicRanges[i].start.row <= rangeEndThreshold) &&
			(atomicRanges[i].end.row > rangeEndThreshold))
		{
			range.end.row = std::max(range.end.row, atomicRanges[i].end.row);
			range.end.strong = true;
		}
	}
	return range;
}

