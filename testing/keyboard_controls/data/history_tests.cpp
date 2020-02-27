
#define HISTORY_TESTS

// TEST: Graph focus restored on back
// START ----------------------------------------------------------------------

class HistoryTest // <- ACTION: activate
{
public:
	int member;
	int member2;
};

int func()
{
	HistoryTest h;
	h.member = 0;
}

// ACTION: Focus and activate 'func' in graph
// ACTION: Navigate 'back' in history
// RESULT: The graph for 'HistoryTest' is restored with focus at 'func'

// END ------------------------------------------------------------------------



// TEST: Graph aggregation focus restored on back
// START ----------------------------------------------------------------------

class HistoryTestUser
{
public:
	int process()
	{
		h.member = 2;
		h.member2 = 4;
	}

	HistoryTest h;
};

// ACTION: Focus and activate aggregation 'HistoryTestUser -> HistoryTest'
// ACTION: Navigate 'back' in history
// RESULT: The graph for 'HistoryTest' is restored with focus at aggregation edge

// END ------------------------------------------------------------------------



// TEST: Code focus restored on back
// START ----------------------------------------------------------------------

HistoryTest createTest(); // <- ACTION: focus and activate 'createTest' from code

// ACTION: Navigate 'back' in history
// RESULT: The code view is restored with focus at 'createTest'

// END ------------------------------------------------------------------------
