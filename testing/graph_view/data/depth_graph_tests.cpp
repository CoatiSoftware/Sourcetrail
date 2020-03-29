
#define DEPTH_GRAPH_TESTS


// TEST: callee graph
// START ----------------------------------------------------------------------

void level_4_func_1() {};
void level_4_func_2() {};
void level_4_func_3() {};
void level_4_func_4() {};

void level_3_func_1() { level_4_func_1(); level_4_func_2(); };
void level_3_func_2() { level_4_func_1(); level_4_func_2(); level_4_func_3(); };
void level_3_func_3() { level_4_func_2(); level_4_func_3(); level_4_func_4(); };
void level_3_func_4() { level_4_func_3(); level_4_func_4(); };

void level_2_func_1() { level_3_func_1(); level_3_func_2(); };
void level_2_func_2() { level_3_func_3(); level_3_func_4(); };

void level_1_func_1() // <- ACTION 1: activate
{
	level_2_func_1();
	level_2_func_2();
};

// ACTION 2: Show callee graph '<'
// RESULTS 2:
// - horizontal layout using bezier curve edges
// - nodes from level 1 to 4 displayed

// ACTION 3: hover and click edges
// RESULTS 3:
// - hover highlight in focus color
// - click shows source location in code

// ACTION 4: Reduce graph depth to 2
// RESULTS 4:
// - only nodes up to level 3 displayed

// ACTION 5: Click on any node
// RESULT 5: node is activated as usual

// END ------------------------------------------------------------------------



// TEST: caller graph with template and override
// START ----------------------------------------------------------------------

void level_3_func_5()
{
	level_4_func_4(); // <- ACTION 1: activate
}

class Level1
{
public:
	virtual void func() = 0;
};

class Level2
	: public Level1
{
public:
	void func() override { level_3_func_5(); }
};

template<typename T>
class Level2_Template
{
public:
	void func() {
		level_3_func_5();
	}
};

void level_0_func1()
{
	Level1* level;
	level->func();

	Level2_Template<int> levelTemplate;
	levelTemplate.func();
}

// ACTION 2: Increase graph depth above 10
// ACTION 3: Show caller graph '>'
// RESULTS 3:
// - horizontal layout using bezier curve edges
// - nodes from level 0 to 4 displayed
// - layout contains override edge shown horizontally
// - layout contains eplicit template class (dead end)
// - layout contains implicit template class

// END ------------------------------------------------------------------------



// TEST: inheritance graph
// START ----------------------------------------------------------------------

class Level_1_Class_1 {}; // <- ACTION 1: activate

class Level_2_Class_1 : public Level_1_Class_1 {};
class Level_2_Class_2 : public Level_1_Class_1 {};

template <typename T>
class Level_3_Class_1 : public Level_2_Class_1 {};

class Level_3_Class_2 : public Level_2_Class_1 {};
class Level_3_Class_3 : public Level_2_Class_1 {};
class Level_3_Class_4 : public Level_2_Class_1 {};

class Level_4_Class_2
	: public Level_3_Class_2
	, public Level_3_Class_3
	, public Level_3_Class_4
{};

class Level_3_Class_5 : public Level_2_Class_2 {};
class Level_3_Class_6 : public Level_2_Class_2 {};

// ACTION 2: Show derived graph '^'
// RESULTS 2:
// - vertical layout using bezier curve edges
// - nodes from level 1 to 4 displayed
// - layout contains template specialization
// - there are 2 framed groups in the graph

// ACTION 3: click on one framed group
// ACTION 4: click on an edge connected to a framed group
// RESULT 3 & 4: The groups are split


class Level_4_Class_1 // <- ACTION 5: activate
	: public Level_3_Class_5
	, public Level_3_Class_6
{};

void level_5_func()
{
	Level_3_Class_1<int> a;
	Level_3_Class_1<char> b;
	Level_3_Class_1<bool> c;
}

// ACTION 6: Show base graph 'v'
// RESULTS 6: like before with other direction

// END ------------------------------------------------------------------------


// TEST: virtual nodes removed when moving nodes
// START ----------------------------------------------------------------------

namespace virtual_nodes
{
void func1(); // <- ACTION 1: activate
void func2();
void func3();
void func4();

struct Parent
{
	static void func5();
};

void func1() { func2(); Parent::func5(); }
void func2() { func3(); }
void func3() { func4(); Parent::func5(); }
void func4() { Parent::func5(); }
void func5() { }
}

// ACTION 2: show callee graph
// ACTION 3: move func5 above node func2
// RESULT 3: virtual nodes are removed from the graph routes

// ACTION 4: activate func4
// ACTION 5: undo
// RESULT 5: the depth graph is restored with the virtual nodes removed

// END ------------------------------------------------------------------------


// TEST: pentagram graph
// START ----------------------------------------------------------------------

namespace pentagram
{
void func1(); // <- ACTION 1: activate
void func2();
void func3();
void func4();
void func5();

void func1() { func1(); func2(); func3(); func4(); func5(); }
void func2() { func1(); func2(); func3(); func4(); func5(); }
void func3() { func1(); func2(); func3(); func4(); func5(); }
void func4() { func1(); func2(); func3(); func4(); func5(); }
void func5() { func1(); func2(); func3(); func4(); func5(); }
}

// ACTION 2: show caller and callee graph
// RESULT 2: layout works without crash

// END ------------------------------------------------------------------------



// TEST: include graph
// START ----------------------------------------------------------------------

#include "depth_files/level_5_file_1.h" // <- ACTION 1: activate

// ACTION 2: show includee graph '<'
// RESULT 2: horizontal layout with bezier curve edges

#include "depth_files/level_1_file_1.h" // <- ACTION 3: activate

// ACTION 4: show includer graph '>'
// RESULT 4: same

// END ------------------------------------------------------------------------

