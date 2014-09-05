#include "utility/text/Dictionary.h"

#include "utility/utilityString.h"

std::shared_ptr<Dictionary> Dictionary::getInstance()
{
	std::lock_guard<std::mutex> lockGuard(s_instanceMutex);
	if (!s_instance)
	{
		s_instance = std::shared_ptr<Dictionary>(new Dictionary());
	}
	return s_instance;
}

Dictionary::~Dictionary()
{
}

Id Dictionary::getWordId(const std::string& word)
{
	for (std::unordered_map<Id, std::string>::const_iterator it = m_words.begin(); it != m_words.end(); it++)
	{
		if (it->second == word)
		{
			return it->first;
		}
	}

	m_words.emplace(++s_nextId, word);
	return s_nextId;
}

std::deque<Id> Dictionary::getWordIds(const std::string& wordList, const std::string& delimiter)
{
	std::deque<std::string> words = utility::split<std::deque<std::string>>(wordList, delimiter);
	std::deque<Id> ids;

	for (const std::string& word: words)
	{
		ids.push_back(getWordId(word));
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

Dictionary::Dictionary()
{
}

std::shared_ptr<Dictionary> Dictionary::s_instance;
std::mutex Dictionary::s_instanceMutex;
Id Dictionary::s_nextId = 0;
