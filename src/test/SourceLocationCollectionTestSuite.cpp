#include "catch.hpp"

#include "SourceLocation.h"
#include "SourceLocationCollection.h"
#include "SourceLocationFile.h"

TEST_CASE("source locations get created with other end")
{
	SourceLocationCollection collection;
	const SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath(L"file.c"), 2, 3, 4, 5);

	REQUIRE(a);
	REQUIRE(a->isStartLocation());
	REQUIRE(!a->isEndLocation());

	const SourceLocation* b = a->getOtherLocation();

	REQUIRE(b);
	REQUIRE(!b->isStartLocation());
	REQUIRE(b->isEndLocation());

	REQUIRE(a == b->getOtherLocation());
	REQUIRE(a == b->getStartLocation());
	REQUIRE(a == a->getStartLocation());
	REQUIRE(b == a->getEndLocation());
	REQUIRE(b == b->getEndLocation());
}

TEST_CASE("source locations do not get created with wrong input")
{
	SourceLocationCollection collection;
	SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath(L"file.c"), 2, 3, 2, 1);
	SourceLocation* b = collection.addSourceLocation(LOCATION_TOKEN, 2, {1}, FilePath(L"file.c"), 4, 1, 1, 10);

	REQUIRE(!a);
	REQUIRE(!b);
}

TEST_CASE("source locations get unique id but both ends have the same")
{
	SourceLocationCollection collection;
	SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath(L"file.c"), 1, 1, 1, 1);
	SourceLocation* b = collection.addSourceLocation(LOCATION_TOKEN, 2, {2}, FilePath(L"file.c"), 1, 1, 1, 1);
	SourceLocation* c = collection.addSourceLocation(LOCATION_TOKEN, 3, {3}, FilePath(L"file.c"), 1, 1, 1, 1);

	REQUIRE(1 == collection.getSourceLocationFileCount());
	REQUIRE(3 == collection.getSourceLocationCount());

	REQUIRE(a->getLocationId() == 1);
	REQUIRE(b->getLocationId() == 2);
	REQUIRE(c->getLocationId() == 3);

	REQUIRE(a->getLocationId() != b->getLocationId());
	REQUIRE(b->getLocationId() != c->getLocationId());
	REQUIRE(c->getLocationId() != a->getLocationId());

	REQUIRE(a->getLocationId() == a->getOtherLocation()->getLocationId());
	REQUIRE(b->getLocationId() == b->getOtherLocation()->getLocationId());
	REQUIRE(c->getLocationId() == c->getOtherLocation()->getLocationId());
}

TEST_CASE("source locations have right file path line column and token id")
{
	SourceLocationCollection collection;
	SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath(L"file.c"), 2, 3, 4, 5);

	REQUIRE(1 == a->getTokenIds()[0]);
	REQUIRE(2 == a->getLineNumber());
	REQUIRE(3 == a->getColumnNumber());
	REQUIRE(4 == a->getOtherLocation()->getLineNumber());
	REQUIRE(5 == a->getOtherLocation()->getColumnNumber());
	REQUIRE(L"file.c" == a->getFilePath().wstr());
}

TEST_CASE("finding source locations by id")
{
	SourceLocationCollection collection;
	SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath(L"file.c"), 2, 3, 4, 5);
	SourceLocation* b = collection.addSourceLocation(LOCATION_TOKEN, 2, {6}, FilePath(L"file.c"), 7, 8, 9, 10);

	REQUIRE(a == collection.getSourceLocationById(a->getLocationId()));
	REQUIRE(b == collection.getSourceLocationById(b->getLocationId()));
}

TEST_CASE("creating plain copy of all locations in line range")
{
	SourceLocationCollection collection;
	SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath(L"file.c"), 2, 3, 4, 5);
	SourceLocation* b = collection.addSourceLocation(LOCATION_TOKEN, 2, {1}, FilePath(L"file.c"), 3, 3, 4, 5);
	SourceLocation* c = collection.addSourceLocation(LOCATION_TOKEN, 3, {1}, FilePath(L"file.c"), 1, 3, 5, 5);
	SourceLocation* d = collection.addSourceLocation(LOCATION_TOKEN, 4, {1}, FilePath(L"file.c"), 1, 5, 4, 5);

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

	REQUIRE(1 == copy.getSourceLocationFileCount());
	REQUIRE(3 == copy.getSourceLocationCount());

	REQUIRE(copy.getSourceLocationById(ida));
	REQUIRE(copy.getSourceLocationById(idb));
	REQUIRE(!copy.getSourceLocationById(idc));
	REQUIRE(copy.getSourceLocationById(idd));

	REQUIRE(a != copy.getSourceLocationById(ida));
	REQUIRE(d != copy.getSourceLocationById(idd));

	REQUIRE(copy.getSourceLocationById(ida)->getStartLocation());
	REQUIRE(copy.getSourceLocationById(ida)->getEndLocation());

	REQUIRE(!copy.getSourceLocationById(idd)->getStartLocation());
	REQUIRE(copy.getSourceLocationById(idd)->getEndLocation());
}

TEST_CASE("get source locations filtered by lines")
{
	SourceLocationCollection collection;
	SourceLocation* a = collection.addSourceLocation(LOCATION_TOKEN, 1, {1}, FilePath(L"file.c"), 1, 3, 1, 5);
	SourceLocation* b = collection.addSourceLocation(LOCATION_TOKEN, 2, {1}, FilePath(L"file.c"), 1, 3, 2, 5);
	SourceLocation* c = collection.addSourceLocation(LOCATION_TOKEN, 3, {1}, FilePath(L"file.c"), 2, 3, 2, 5);
	SourceLocation* d = collection.addSourceLocation(LOCATION_TOKEN, 4, {1}, FilePath(L"file.c"), 3, 3, 4, 5);
	SourceLocation* e = collection.addSourceLocation(LOCATION_TOKEN, 5, {1}, FilePath(L"file.c"), 3, 5, 5, 5);
	SourceLocation* f = collection.addSourceLocation(LOCATION_TOKEN, 6, {1}, FilePath(L"file.c"), 1, 5, 5, 5);
	SourceLocation* g = collection.addSourceLocation(LOCATION_TOKEN, 7, {1}, FilePath(L"file.c"), 5, 5, 5, 5);

	SourceLocationCollection copy;
	copy.addSourceLocationFile(
		collection.getSourceLocationById(a->getLocationId())->getSourceLocationFile()->getFilteredByLines(2, 4));

	REQUIRE(1 == copy.getSourceLocationFileCount());
	REQUIRE(4 == copy.getSourceLocationCount());

	REQUIRE(!copy.getSourceLocationById(a->getLocationId()));
	REQUIRE(copy.getSourceLocationById(b->getLocationId()));
	REQUIRE(copy.getSourceLocationById(c->getLocationId()));
	REQUIRE(copy.getSourceLocationById(d->getLocationId()));
	REQUIRE(copy.getSourceLocationById(e->getLocationId()));
	REQUIRE(!copy.getSourceLocationById(f->getLocationId()));
	REQUIRE(!copy.getSourceLocationById(g->getLocationId()));

	REQUIRE(b != copy.getSourceLocationById(b->getLocationId()));
	REQUIRE(c != copy.getSourceLocationById(c->getLocationId()));

	REQUIRE(!copy.getSourceLocationById(b->getLocationId())->getStartLocation());
	REQUIRE(copy.getSourceLocationById(b->getLocationId())->getEndLocation());

	REQUIRE(copy.getSourceLocationById(e->getLocationId())->getStartLocation());
	REQUIRE(!copy.getSourceLocationById(e->getLocationId())->getEndLocation());
}
