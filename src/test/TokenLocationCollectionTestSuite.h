#include "cxxtest/TestSuite.h"

#include "data/location/TokenLocation.h"
#include "data/location/TokenLocationCollection.h"
#include "data/location/TokenLocationFile.h"
#include "data/location/TokenLocationLine.h"

class TokenLocationCollectionTestSuite : public CxxTest::TestSuite
{
public:
	void test_token_locations_get_created_with_other_end()
	{
		TokenLocationCollection collection;
		TokenLocation* a = collection.addTokenLocation(1, 1, "file.c", 2, 3, 4, 5);

		TS_ASSERT(a);
		TS_ASSERT(a->isStartTokenLocation());
		TS_ASSERT(!a->isEndTokenLocation());

		TokenLocation* b = a->getOtherTokenLocation();

		TS_ASSERT(b);
		TS_ASSERT(!b->isStartTokenLocation());
		TS_ASSERT(b->isEndTokenLocation());

		TS_ASSERT_EQUALS(a, b->getOtherTokenLocation());
		TS_ASSERT_EQUALS(a, b->getStartTokenLocation());
		TS_ASSERT_EQUALS(a, a->getStartTokenLocation());
		TS_ASSERT_EQUALS(b, a->getEndTokenLocation());
		TS_ASSERT_EQUALS(b, b->getEndTokenLocation());
	}

	void test_token_locations_do_not_get_created_with_wrong_input()
	{
		TokenLocationCollection collection;
		TokenLocation* a = collection.addTokenLocation(1, 1, "file.c", 2, 3, 2, 1);
		TokenLocation* b = collection.addTokenLocation(2, 1, "file.c", 4, 1, 1, 10);

		TS_ASSERT(!a);
		TS_ASSERT(!b);
	}

	void test_token_locations_get_unique_id_but_both_ends_have_the_same()
	{
		TokenLocationCollection collection;
		TokenLocation* a = collection.addTokenLocation(1, 1, "file.c", 1, 1, 1, 1);
		TokenLocation* b = collection.addTokenLocation(2, 2, "file.c", 1, 1, 1, 1);
		TokenLocation* c = collection.addTokenLocation(3, 3, "file.c", 1, 1, 1, 1);

		TS_ASSERT_EQUALS(1, collection.getTokenLocationFileCount());
		TS_ASSERT_EQUALS(3, collection.getTokenLocationCount());

		TS_ASSERT_DIFFERS(a->getId(), b->getId());
		TS_ASSERT_DIFFERS(b->getId(), c->getId());
		TS_ASSERT_DIFFERS(c->getId(), a->getId());

		TS_ASSERT_EQUALS(a->getId(), a->getOtherTokenLocation()->getId());
		TS_ASSERT_EQUALS(b->getId(), b->getOtherTokenLocation()->getId());
		TS_ASSERT_EQUALS(c->getId(), c->getOtherTokenLocation()->getId());
	}

	void test_token_locations_have_right_file_path_line_column_and_token_id()
	{
		TokenLocationCollection collection;
		TokenLocation* a = collection.addTokenLocation(1, 1, "file.c", 2, 3, 4, 5);

		TS_ASSERT_EQUALS(1, a->getTokenId());
		TS_ASSERT_EQUALS(2, a->getLineNumber());
		TS_ASSERT_EQUALS(3, a->getColumnNumber());
		TS_ASSERT_EQUALS(4, a->getOtherTokenLocation()->getLineNumber());
		TS_ASSERT_EQUALS(5, a->getOtherTokenLocation()->getColumnNumber());
		TS_ASSERT_EQUALS("file.c", a->getFilePath().str());
	}

	void test_finding_token_locations_by_id()
	{
		TokenLocationCollection collection;
		TokenLocation* a = collection.addTokenLocation(1, 1, "file.c", 2, 3, 4, 5);
		TokenLocation* b = collection.addTokenLocation(2, 6, "file.c", 7, 8, 9, 10);

		TS_ASSERT_EQUALS(a, collection.findTokenLocationById(a->getId()));
		TS_ASSERT_EQUALS(b, collection.findTokenLocationById(b->getId()));
	}

	void test_removing_token_locations()
	{
		TokenLocationCollection collection;
		TokenLocation* a = collection.addTokenLocation(1, 1, "file.c", 2, 3, 4, 5);
		TokenLocation* b = collection.addTokenLocation(2, 1, "file.c", 3, 3, 4, 5);
		TokenLocation* c = collection.addTokenLocation(3, 1, "file.c", 1, 3, 5, 5);
		TokenLocation* d = collection.addTokenLocation(4, 1, "file2.c", 1, 3, 5, 5);

		TS_ASSERT_EQUALS(2, collection.getTokenLocationFileCount());
		TS_ASSERT_EQUALS(4, collection.getTokenLocationCount());
		TS_ASSERT_EQUALS(5, c->getTokenLocationFile()->getTokenLocationLineCount());

		Id ida = a->getId();
		Id idb = b->getId();
		Id idc = c->getId();

		collection.removeTokenLocation(a);
		collection.removeTokenLocation(b->getOtherTokenLocation());
		collection.removeTokenLocation(d);

		TS_ASSERT(!collection.findTokenLocationById(ida));
		TS_ASSERT(!collection.findTokenLocationById(idb));
		TS_ASSERT_EQUALS(c, collection.findTokenLocationById(idc));

		TS_ASSERT_EQUALS(1, collection.getTokenLocationFileCount());
		TS_ASSERT_EQUALS(1, collection.getTokenLocationCount());
		TS_ASSERT_EQUALS(2, c->getTokenLocationFile()->getTokenLocationLineCount());
	}

	void test_creating_plain_copy_of_all_locations_in_line_range()
	{
		TokenLocationCollection collection;
		TokenLocation* a = collection.addTokenLocation(1, 1, "file.c", 2, 3, 4, 5);
		TokenLocation* b = collection.addTokenLocation(2, 1, "file.c", 3, 3, 4, 5);
		TokenLocation* c = collection.addTokenLocation(3, 1, "file.c", 1, 3, 5, 5);
		TokenLocation* d = collection.addTokenLocation(4, 1, "file.c", 1, 5, 4, 5);

		Id ida = a->getId();
		Id idb = b->getId();
		Id idc = c->getId();
		Id idd = d->getId();

		unsigned int fromLine = 2;
		unsigned int toLine = 4;

		TokenLocationCollection copy;
		TokenLocation* x = collection.findTokenLocationById(ida);

		x->getTokenLocationFile()->forEachTokenLocationLine([&copy, fromLine, toLine](TokenLocationLine* line)
		{
			unsigned int l = line->getLineNumber();
			if (l >= fromLine && l <= toLine)
			{
				line->forEachTokenLocation([&copy](TokenLocation* location)
				{
					copy.addTokenLocationAsPlainCopy(location);
				});
			}
		});

		TS_ASSERT_EQUALS(1, copy.getTokenLocationFileCount());
		TS_ASSERT_EQUALS(3, copy.getTokenLocationCount());

		TS_ASSERT(copy.findTokenLocationById(ida));
		TS_ASSERT(copy.findTokenLocationById(idb));
		TS_ASSERT(!copy.findTokenLocationById(idc));
		TS_ASSERT(copy.findTokenLocationById(idd));

		TS_ASSERT_DIFFERS(a, copy.findTokenLocationById(ida));
		TS_ASSERT_DIFFERS(d, copy.findTokenLocationById(idd));

		TS_ASSERT(copy.findTokenLocationById(ida)->getStartTokenLocation());
		TS_ASSERT(copy.findTokenLocationById(ida)->getEndTokenLocation());

		TS_ASSERT(!copy.findTokenLocationById(idd)->getStartTokenLocation());
		TS_ASSERT(copy.findTokenLocationById(idd)->getEndTokenLocation());
	}
};
