#include "component/controller/helper/SnippetMerger.h"

#include <algorithm>
#include "settings/ApplicationSettings.h"

SnippetMerger::SnippetMerger(int startRow, int endRow)
	: m_start(startRow)
	, m_end(endRow)
{
}

void SnippetMerger::addChild(std::shared_ptr<SnippetMerger> child)
{
	m_children.push_back(child);
}

std::deque<SnippetMerger::Range> SnippetMerger::merge() const
{
	std::deque<Range> merged;
	if (m_children.size() == 0)
	{
		merged.push_back(Range(Border(m_start, false), Border(m_end, false)));
	}
	else
	{
		for (size_t i = 0; i < m_children.size(); i++)
		{
			std::deque<Range> mergedFromChild = m_children[i]->merge();
			for (size_t j = 0; j < mergedFromChild.size(); j++)
			{
				merged.push_back(mergedFromChild[j]);
			}
		}
		std::sort(merged.begin(), merged.end(),
			[](const Range& a, const Range& b)
			{
				return a.start.row < b.start.row;
			}
		);

		// merge children
		const int snippetExpandRange = ApplicationSettings::getInstance()->getCodeSnippetExpandRange();
		const int snippetMergeRange = 2 * snippetExpandRange + 1;	// +1 since snippets that end/start with consequtive 
																	// lines should be merged as well.
		for (size_t i = 0; i < merged.size() - 1; i++)
		{
			const Range first = merged[i];
			const Range second = merged[i + 1];
			if (first.end.row + snippetMergeRange >= second.start.row)
			{
				merged.erase(merged.begin() + i, merged.begin() + i + 2);
				merged.insert(merged.begin() + i, Range(
					first.start.row < second.start.row ? first.start : second.start,
					first.end.row > second.end.row ? first.end : second.end
				));
				i--;
			}
		}

		// snap to own borders
		const int snippetSnapRange = ApplicationSettings::getInstance()->getCodeSnippetSnapRange();
		if (m_start + snippetSnapRange >= merged.front().start.row)
		{
			merged.front().start.row = m_start;
			merged.front().start.strong = true;
		}
		if (m_end - snippetSnapRange <= merged.back().end.row)
		{
			merged.back().end.row = m_end;
			merged.back().end.strong = true;
		}
	}
	return merged;
}
