#ifndef SUFFIX_ARRAY_H
#define SUFFIX_ARRAY_H

#include <vector>
#include <string>
#include <iostream>

class SuffixArray
{
public:
	SuffixArray(const std::string& text);
	std::vector<int> searchForTerm(const std::string& searchTerm) const;
	static int cmp(struct suffix a, struct suffix b);

	void printArray() const;
	void printLCP() const;

private:
	template <typename T>
	void printArr(std::vector<T>arr) const
	{
		for (size_t i = 0; i < arr.size(); i++)
		{
			std::cout << arr[i] << " ";
		}
		std::cout << std::endl;
	}

	std::vector<int> buildLCP();
	std::vector<int> buildSuffixArray();
	std::vector<int> m_array;
	std::vector<int> m_lcp;
	std::string m_text;
};

#endif // SUFFIX_ARRAY_H
