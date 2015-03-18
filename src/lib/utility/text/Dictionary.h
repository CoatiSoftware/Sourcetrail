#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <deque>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "utility/types.h"

class Dictionary
{
public:
	Dictionary();
	~Dictionary();

	void clear();

	Id getWordId(const std::string& word);
	Id getWordIdConst(const std::string& word) const;

	std::deque<Id> getWordIds(const std::string& wordList, const std::string& delimiter);
	std::deque<Id> getWordIdsConst(const std::string& wordList, const std::string& delimiter) const;

	// Note: References to values in an unordered_map don't change on rehashing so they can be saved and used elsewhere.
	const std::string& getWord(Id id) const;
	std::string getWord(const std::deque<Id> ids, const std::string& delimiter) const;

private:
	static Id s_nextId;

	std::unordered_map<Id, std::string> m_words;
	std::unordered_map<std::string, Id> m_ids; // TODO: replace id lookup to consume less momory e.g. Search Trie
	std::string m_emptyWord;
};

#endif // DICTIONARY_H
