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
	static std::shared_ptr<Dictionary> getInstance();
	~Dictionary();

	Id getWordId(const std::string& word);
	std::deque<Id> getWordIds(const std::string& wordList, const std::string& delimiter);

	// Note: References to values in an unordered_map don't change on rehashing so they can be saved and used elsewhere.
	const std::string& getWord(Id id) const;
	std::string getWord(const std::deque<Id> ids, const std::string& delimiter) const;

private:
	Dictionary();
	Dictionary(const Dictionary&);
	void operator=(const Dictionary&);

	static std::shared_ptr<Dictionary> s_instance;
	static std::mutex s_instanceMutex;
	static Id s_nextId;

	std::unordered_map<Id, std::string> m_words;
	std::string m_emptyWord;
};

#endif // DICTIONARY_H
