#include "SuffixArray.h"

#include <algorithm>
#include <iostream>

struct suffix
{
	int index;
	int rank[2];
};

int SuffixArray::cmp(const struct suffix& a, const struct suffix& b)
{
	return (a.rank[0] == b.rank[0]) ? (a.rank[1] < b.rank[1] ? 1 : 0)
									: (a.rank[0] < b.rank[0] ? 1 : 0);
}

SuffixArray::SuffixArray(const std::wstring& text): m_text(text)
{
	std::transform(m_text.begin(), m_text.end(), m_text.begin(), ::towlower);
	m_array = buildSuffixArray();
	m_lcp = buildLCP();
}

void SuffixArray::printArray() const
{
	std::cout << "Suffix Array : \n";
	printArr(m_array);
	for (size_t i = 0; i < m_array.size(); i++)
	{
		std::wstring suffix = m_text.substr(m_array[i]);
		std::wcout << i << ": \"" << suffix << "\"" << std::endl;
	}
}

void SuffixArray::printLCP() const
{
	std::cout << "\nLCP Array : \n";
	printArr(m_lcp);
	for (size_t i = 0; i < m_array.size(); i++)
	{
		std::wstring prefix = m_text.substr(m_array[i], m_lcp[i]);
		std::wcout << i << ": \"" << prefix << "\"" << std::endl;
	}
}

std::vector<int> SuffixArray::buildLCP()
{
	const int n = static_cast<int>(m_array.size());

	std::vector<int> lcp(n, 0);
	std::vector<int> invSuff(n, 0);

	for (int i = 0; i < n; i++)
	{
		invSuff[m_array[i]] = i;
	}

	int k = 0;

	for (int i = 0; i < n; i++)
	{
		if (invSuff[i] == n - 1)
		{
			k = 0;
			continue;
		}

		int j = m_array[invSuff[i] + 1];

		while (i + k < n && j + k < n && m_text[i + k] == m_text[j + k])
		{
			k++;
		}

		lcp[invSuff[i]] = k;

		if (k > 0)
		{
			k--;
		}
	}

	return lcp;
}

std::vector<int> SuffixArray::searchForTerm(const std::wstring& searchTerm) const
{
	std::wstring term = searchTerm;
	std::transform(term.begin(), term.end(), term.begin(), ::towlower);

	const int termLength = static_cast<int>(term.length());
	const int textLength = static_cast<int>(m_text.length());
	int l = -1;
	int r = textLength;
	int m;

	std::vector<int> matches;
	int compareResult;
	while (l + 1 < r)
	{
		m = (l + r + 1) / 2;
		compareResult = term.compare(m_text.substr(m_array[m], termLength));
		if (compareResult < 0)
		{
			r = m;
		}
		else if (compareResult > 0)
		{
			l = m;
		}
		else
		{
			matches.push_back(m_array[m]);
			for (int lower = m - 1; lower >= 0 && m_lcp[lower] >= termLength; lower--)
			{
				matches.push_back(m_array[lower]);
			}
			for (int higher = m + 1; higher < textLength && m_lcp[higher - 1] >= termLength; higher++)
			{
				matches.push_back(m_array[higher]);
			}
			break;
		}
	}

	std::sort(matches.begin(), matches.end());

	return matches;
}

std::vector<int> SuffixArray::buildSuffixArray()
{
	const int n = static_cast<int>(m_text.length());
	std::vector<suffix> suffixes;
	suffixes.reserve(n);

	suffix s;
	for (int i = 0; i < n; i++)
	{
		s.index = i;
		s.rank[0] = m_text[i];
		s.rank[1] = ((i + 1) < n) ? (m_text[i + 1]) : -1;
		suffixes.push_back(s);
	}

	std::sort(suffixes.begin(), suffixes.end(), SuffixArray::cmp);

	std::vector<int> ind(n, 0);
	for (int k = 4; k < 2 * n; k = k * 2)
	{
		int rank = 0;
		int prev_rank = suffixes[0].rank[0];
		suffixes[0].rank[0] = rank;
		ind[suffixes[0].index] = 0;

		for (int i = 1; i < n; i++)
		{
			if (suffixes[i].rank[0] == prev_rank && suffixes[i].rank[1] == suffixes[i - 1].rank[1])
			{
				prev_rank = suffixes[i].rank[0];
				suffixes[i].rank[0] = rank;
			}
			else
			{
				prev_rank = suffixes[i].rank[0];
				suffixes[i].rank[0] = ++rank;
			}
			ind[suffixes[i].index] = i;
		}

		for (int i = 0; i < n; i++)
		{
			int nextindex = suffixes[i].index + k / 2;
			suffixes[i].rank[1] = (nextindex < n) ? suffixes[ind[nextindex]].rank[0] : -1;
		}

		std::sort(suffixes.begin(), suffixes.end(), cmp);
	}

	std::vector<int> suffixArr;
	for (int i = 0; i < n; i++)
	{
		suffixArr.push_back(suffixes[i].index);
	}

	return suffixArr;
}
