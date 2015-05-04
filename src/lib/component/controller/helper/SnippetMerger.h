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
		Range (Border start, Border end): start(start), end(end) {}
		Border start;
		Border end;
	};

	SnippetMerger(int startRow, int endRow);
	void addChild(std::shared_ptr<SnippetMerger> child);
	std::deque<Range> merge() const;

private:
	const int m_start;
	const int m_end;
	std::vector<std::shared_ptr<SnippetMerger>> m_children;
};

#endif // SNIPPET_MERGER_H
