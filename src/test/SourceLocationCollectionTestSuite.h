#include "cxxtest/TestSuite.h"

#include "data/location/SourceLocation.h"
#include "data/location/SourceLocationCollection.h"
#include "data/location/SourceLocationFile.h"

class SourceLocationCollectionTestSuite : public CxxTest::TestSuite
{
public:
	void test_source_locations_get_created_with_other_end()
	{
		SourceLocationCollection collection;
		const SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath("file.c"), 2, 3, 4, 5);

		TS_ASSERT(a);
		TS_ASSERT(a->isStartLocation());
		TS_ASSERT(!a->isEndLocation());

		const SourceLocation* b = a->getOtherLocation();

		TS_ASSERT(b);
		TS_ASSERT(!b->isStartLocation());
		TS_ASSERT(b->isEndLocation());

		TS_ASSERT_EQUALS(a, b->getOtherLocation());
		TS_ASSERT_EQUALS(a, b->getStartLocation());
		TS_ASSERT_EQUALS(a, a->getStartLocation());
		TS_ASSERT_EQUALS(b, a->getEndLocation());
		TS_ASSERT_EQUALS(b, b->getEndLocation());
	}

	void test_source_locations_do_not_get_created_with_wrong_input()
	{
		SourceLocationCollection collection;
		SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath("file.c"), 2, 3, 2, 1);
		SourceLocation* b = collection.addSourceLocation(LOCATION_TOKEN, 2, {1}, FilePath("file.c"), 4, 1, 1, 10);

		TS_ASSERT(!a);
		TS_ASSERT(!b);
	}

	void test_source_locations_get_unique_id_but_both_ends_have_the_same()
	{
		SourceLocationCollection collection;
		SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath("file.c"), 1, 1, 1, 1);
		SourceLocation* b = collection.addSourceLocation(LOCATION_TOKEN, 2, {2}, FilePath("file.c"), 1, 1, 1, 1);
		SourceLocation* c = collection.addSourceLocation(LOCATION_TOKEN, 3, {3}, FilePath("file.c"), 1, 1, 1, 1);

		TS_ASSERT_EQUALS(1, collection.getSourceLocationFileCount());
		TS_ASSERT_EQUALS(3, collection.getSourceLocationCount());

		TS_ASSERT_EQUALS(a->getLocationId(), 1);
		TS_ASSERT_EQUALS(b->getLocationId(), 2);
		TS_ASSERT_EQUALS(c->getLocationId(), 3);

		TS_ASSERT_DIFFERS(a->getLocationId(), b->getLocationId());
		TS_ASSERT_DIFFERS(b->getLocationId(), c->getLocationId());
		TS_ASSERT_DIFFERS(c->getLocationId(), a->getLocationId());

		TS_ASSERT_EQUALS(a->getLocationId(), a->getOtherLocation()->getLocationId());
		TS_ASSERT_EQUALS(b->getLocationId(), b->getOtherLocation()->getLocationId());
		TS_ASSERT_EQUALS(c->getLocationId(), c->getOtherLocation()->getLocationId());
	}

	void test_source_locations_have_right_file_path_line_column_and_token_id()
	{
		SourceLocationCollection collection;
		SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath("file.c"), 2, 3, 4, 5);

		TS_ASSERT_EQUALS(1, a->getTokenIds()[0]);
		TS_ASSERT_EQUALS(2, a->getLineNumber());
		TS_ASSERT_EQUALS(3, a->getColumnNumber());
		TS_ASSERT_EQUALS(4, a->getOtherLocation()->getLineNumber());
		TS_ASSERT_EQUALS(5, a->getOtherLocation()->getColumnNumber());
		TS_ASSERT_EQUALS("file.c", a->getFilePath().str());
	}

	void test_finding_source_locations_by_id()
	{
		SourceLocationCollection collection;
		SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath("file.c"), 2, 3, 4, 5);
		SourceLocation* b = collection.addSourceLocation(LOCATION_TOKEN, 2, {6}, FilePath("file.c"), 7, 8, 9, 10);

		TS_ASSERT_EQUALS(a, collection.getSourceLocationById(a->getLocationId()));
		TS_ASSERT_EQUALS(b, collection.getSourceLocationById(b->getLocationId()));
	}

	void test_creating_plain_copy_of_all_locations_in_line_range()
	{
		SourceLocationCollection collection;
		SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath("file.c"), 2, 3, 4, 5);
		SourceLocation* b = collection.addSourceLocation(LOCATION_TOKEN, 2, {1}, FilePath("file.c"), 3, 3, 4, 5);
		SourceLocation* c = collection.addSourceLocation(LOCATION_TOKEN, 3, {1}, FilePath("file.c"), 1, 3, 5, 5);
		SourceLocation* d = collection.addSourceLocation(LOCATION_TOKEN, 4, {1}, FilePath("file.c"), 1, 5, 4, 5);

		Id ida = a->getLocationId();
		Id idb = b->getLocationId();
		Id idc = c->getLocationId();
		Id idd = d->getLocationId();

		unsigned int fromLine = 2;
		unsigned int toLine = 4;

		SourceLocationCollection copy;
		SourceLocation* x = collection.getSourceLocationById(ida);

		x->getSourceLocationFile()->forEachSourceLocation(
			[&copy, fromLine, toLine](SourceLocation* location)
			{
				if (location->getLineNumber() >= fromLine && location->getLineNumber() <= toLine)
				{
					copy.addSourceLocationCopy(location);
				}
			}
		);

		TS_ASSERT_EQUALS(1, copy.getSourceLocationFileCount());
		TS_ASSERT_EQUALS(3, copy.getSourceLocationCount());

		TS_ASSERT(copy.getSourceLocationById(ida));
		TS_ASSERT(copy.getSourceLocationById(idb));
		TS_ASSERT(!copy.getSourceLocationById(idc));
		TS_ASSERT(copy.getSourceLocationById(idd));

		TS_ASSERT_DIFFERS(a, copy.getSourceLocationById(ida));
		TS_ASSERT_DIFFERS(d, copy.getSourceLocationById(idd));

		TS_ASSERT(copy.getSourceLocationById(ida)->getStartLocation());
		TS_ASSERT(copy.getSourceLocationById(ida)->getEndLocation());

		TS_ASSERT(!copy.getSourceLocationById(idd)->getStartLocation());
		TS_ASSERT(copy.getSourceLocationById(idd)->getEndLocation());
	}

	void test_get_source_locations_filtered_by_lines()
	{
		SourceLocationCollection collection;
		SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath("file.c"), 1, 3, 1, 5);
		SourceLocation* b = collection.addSourceLocation(LOCATION_TOKEN, 2, {1}, FilePath("file.c"), 1, 3, 2, 5);
		SourceLocation* c = collection.addSourceLocation(LOCATION_TOKEN, 3, {1}, FilePath("file.c"), 2, 3, 2, 5);
		SourceLocation* d = collection.addSourceLocation(LOCATION_TOKEN, 4, {1}, FilePath("file.c"), 3, 3, 4, 5);
		SourceLocation* e = collection.addSourceLocation(LOCATION_TOKEN, 5, {1}, FilePath("file.c"), 3, 5, 5, 5);
		SourceLocation* f = collection.addSourceLocation(LOCATION_TOKEN, 6, {1}, FilePath("file.c"), 1, 5, 5, 5);
		SourceLocation* g = collection.addSourceLocation(LOCATION_TOKEN, 7, {1}, FilePath("file.c"), 5, 5, 5, 5);

		SourceLocationCollection copy;
		copy.addSourceLocationFile(
			collection.getSourceLocationById(a->getLocationId())->getSourceLocationFile()->getFilteredByLines(2, 4));

		TS_ASSERT_EQUALS(1, copy.getSourceLocationFileCount());
		TS_ASSERT_EQUALS(4, copy.getSourceLocationCount());

		TS_ASSERT(!copy.getSourceLocationById(a->getLocationId()));
		TS_ASSERT(copy.getSourceLocationById(b->getLocationId()));
		TS_ASSERT(copy.getSourceLocationById(c->getLocationId()));
		TS_ASSERT(copy.getSourceLocationById(d->getLocationId()));
		TS_ASSERT(copy.getSourceLocationById(e->getLocationId()));
		TS_ASSERT(!copy.getSourceLocationById(f->getLocationId()));
		TS_ASSERT(!copy.getSourceLocationById(g->getLocationId()));

		TS_ASSERT_DIFFERS(b, copy.getSourceLocationById(b->getLocationId()));
		TS_ASSERT_DIFFERS(c, copy.getSourceLocationById(c->getLocationId()));

		TS_ASSERT(!copy.getSourceLocationById(b->getLocationId())->getStartLocation());
		TS_ASSERT(copy.getSourceLocationById(b->getLocationId())->getEndLocation());

		TS_ASSERT(copy.getSourceLocationById(e->getLocationId())->getStartLocation());
		TS_ASSERT(!copy.getSourceLocationById(e->getLocationId())->getEndLocation());
	}
};
