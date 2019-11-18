#ifndef SNIPPET_MERGER_H
#define SNIPPET_MERGER_H

#include <deque>
#include <memory>
#include <vector>

class SnippetMerger
{
public:
	struct Border
	{
		Border(int row, bool strong): row(row), strong(strong) {}
		int row;
		bool strong;
	};
	struct Range
	{
		template <template <class, class> class ContainerType>
		static ContainerType<Range, std::allocator<Range>> mergeAdjacent(
			ContainerType<Range, std::allocator<Range>> ranges, int rowDifference = 1)
		{
			for (size_t i = 0; i + 1 < ranges.size(); i++)
			{
				const Range first = ranges[i];
				const Range second = ranges[i + 1];
				if (first.end.row + rowDifference >= second.start.row)
				{
					ranges.erase(ranges.begin() + i, ranges.begin() + i + 2);
					ranges.insert(
						ranges.begin() + i,
						Range(
							first.start.row < second.start.row ? first.start : second.start,
							first.end.row > second.end.row ? first.end : second.end));
					i--;
				}
			}
			return ranges;
		}

		Range(Border start, Border end): start(start), end(end) {}
		Border start;
		Border end;
	};

	SnippetMerger(int startRow, int endRow);
	void addChild(std::shared_ptr<SnippetMerger> child);
	std::deque<Range> merge(std::vector<SnippetMerger::Range> atomicRanges) const;

private:
	Range getExpandedRegardingAtomicRanges(
		Range range, const int snippetExpandRange, const std::vector<Range>& atomicRanges) const;

	const int m_start;
	const int m_end;
	std::vector<std::shared_ptr<SnippetMerger>> m_children;
};

#endif	  // SNIPPET_MERGER_H
