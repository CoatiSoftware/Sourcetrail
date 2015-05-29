#include "utility/text/Dictionary.h"

#include "utility/utilityString.h"

Dictionary::Dictionary()
{
}

Dictionary::~Dictionary()
{
}

void Dictionary::clear()
{
	m_words.clear();
	m_ids.clear();
}

size_t Dictionary::getCharCount() const
{
	size_t count = 0;

	for (const std::pair<Id, std::string>& p : m_words)
	{
		count += p.second.size();
	}

	return count;
}

size_t Dictionary::getWordCount() const
{
	return m_words.size();
}

Id Dictionary::getWordId(const std::string& word)
{
	Id wordId = getWordIdConst(word);
	if (wordId)
	{
		return wordId;
	}

	m_words.emplace(++s_nextId, word);
	m_ids.emplace(word, s_nextId);
	return s_nextId;
}

Id Dictionary::getWordIdConst(const std::string& word) const
{
	std::unordered_map<std::string, Id>::const_iterator it = m_ids.find(word);

	if (it != m_ids.end())
	{
		return it->second;
	}

	return 0;
}

std::deque<Id> Dictionary::getWordIds(const std::string& wordList, const std::string& delimiter)
{
	std::deque<std::string> words = utility::split(wordList, delimiter);
	std::deque<Id> ids;

	for (const std::string& word: words)
	{
		ids.push_back(getWordId(word));
	}

	return ids;
}

std::deque<Id> Dictionary::getWordIdsConst(const std::string& wordList, const std::string& delimiter) const
{
	std::deque<std::string> words = utility::split(wordList, delimiter);
	std::deque<Id> ids;

	for (const std::string& word: words)
	{
		ids.push_back(getWordIdConst(word));
	}

	return ids;
}

const std::string& Dictionary::getWord(Id id) const
{
	std::unordered_map<Id, std::string>::const_iterator it = m_words.find(id);

	if (it != m_words.end())
	{
		return it->second;
	}

	return m_emptyWord;
}

std::string Dictionary::getWord(const std::deque<Id> ids, const std::string& delimiter) const
{
	std::string word;

	for (std::deque<Id>::const_iterator it = ids.begin(); it != ids.end(); it++)
	{
		if (it != ids.begin())
		{
			word += delimiter;
		}

		word += getWord(*it);
	}

	return word;
}

Id Dictionary::s_nextId = 0;
