#include "TestStorage.h"

#include "utility/text/TextAccess.h"
#include "data/parser/cxx/CxxParser.h"

void TestStorage::parseCxxCode(std::string code)
{
	clear();
	CxxParser parser(this);
	parser.parseFile(TextAccess::createFromString(code));
}

const Graph& TestStorage::getGraph() const
{
	return Storage::getGraph();
}
