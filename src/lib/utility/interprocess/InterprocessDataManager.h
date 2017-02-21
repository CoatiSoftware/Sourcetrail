#ifndef INTERPROCESS_DATA_MANAGER_H
#define INTERPROCESS_DATA_MANAGER_H

#include "data/parser/Parser.h"

#include "SharedQueue.h"

#include "SharedParserArguments.h"

class InterprocessDataManager
{
public:
	InterprocessDataManager(const bool isOwner = false);
	~InterprocessDataManager();

	void initialize();

	void pushParserArguments(const Parser::Arguments& arguments);
	Parser::Arguments popParserArguments();
	unsigned int parserArgumentCount() const;

private:
	void cleanSharedMemory();

	static std::string m_sharedArgumentQueueName;

	bool m_isOwner;
	bool m_initialized;

	SharedQueue<SharedParserArguments> m_parserArguments;
};

#endif // INTERPROCESS_DATA_MANAGER_H
