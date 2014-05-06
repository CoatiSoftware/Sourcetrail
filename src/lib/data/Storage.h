#ifndef STORAGE_H
#define STORAGE_H

#include <memory>
#include <vector>

#include "data/Graph.h"
#include "data/parser/ParserClient.h"
#include "data/TextLocationFile.h"

class Storage: public ParserClient
{
public:
	Storage();
	~Storage();

	virtual void addClass(const ParseObject& object);

private:
	Graph m_graph;

	std::vector<std::shared_ptr<TextLocationFile> > m_textLocationFiles;
};

#endif // STORAGE_H
