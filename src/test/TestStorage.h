#ifndef TEST_STORAGE_H
#define TEST_STORAGE_H

#include "data/Storage.h"

class TestStorage
	: public Storage
{
public:
	void parseCxxCode(std::string code);
	const Graph& getGraph() const;
};

#endif // TEST_STORAGE_H
