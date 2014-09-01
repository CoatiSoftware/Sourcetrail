#include "utilityTest.h"

#include "data/graph/Graph.h"
#include "data/Storage.h"
#include "utility/text/TextAccess.h"
#include "data/parser/cxx/CxxParser.h"

namespace
{
	class TestStorage
		: public Storage
	{
	public:
		const Graph& getGraph() const
		{
			return Storage::getGraph();
		}
	};
}

Graph utility::getGraphForCxxCode(std::string code)
{
	TestStorage storage;
	CxxParser parser(&storage);
	parser.parseFile(TextAccess::createFromString(code));
	return storage.getGraph();
}
