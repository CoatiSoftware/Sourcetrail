#include <map>
#include "interaction_files/include.h"

#define INTERACTION_TESTS


// TEST: hover nodes
// START ----------------------------------------------------------------------

// ACTION: Activate legend and hover each node in the left column
// RESULT: Each node changes on hover (except group node)

// END ------------------------------------------------------------------------



// TEST: hover edge
// START ----------------------------------------------------------------------

// ACTION: Activate legend and hover each edge in the right column
// RESULT: Each edge changes on hover

// END ------------------------------------------------------------------------



// TEST: expand/collapse nodes
// START ----------------------------------------------------------------------

class CollapsedClass
{
	void foo();
	void bar();
};

class ExpandedClass
{
public:
	void method1();
	void method2();
	void method3();
	int member;

private:
	CollapsedClass field; // <- ACTION1: activate member;
};

// RESULTS 1:
// - ExpandedClass is half expanded (4)
// - CollapsedClass is collapsed (2)

// ACTION 2: Expand CollapsedClass
// RESULT 2: CollapsedClass is expanded (^)

// ACTION 3: Expand ExpandedClass
// RESULT 3: ExpandedClass is expanded (^)

// ACTION 4: Collapse both classes with context menu action
// RESULT 3: both clases are collapsed

// ACTION 5: Click on ExpandedClass
// RESULT 5: ExpandedClass gets fully expanded

// END ------------------------------------------------------------------------



// TEST: click node
// START ----------------------------------------------------------------------

namespace interaction
{
	class User
		: public Class
	{
		void method() // <- ACTION 1: activate function
		{
			member = 1;
			Struct s;
			Enum e = ENUM_CONSTANT;
			TypeDef d;
			using namespace package;
		}
	};
}

// ACTION 2: Click each node in the graph
// RESULT 2: Each node is activated

// END ------------------------------------------------------------------------



// TEST: click namespace
// START ----------------------------------------------------------------------

// ACTION 1: repeat ACTION 1 above

// ACTION 2: hover triangle at 'User'
// RESULT 2: namespace label 'interaction' appears

// ACTION 3: click 'interaction'
// RESULT 3: namespace 'interaction' is active

// END ------------------------------------------------------------------------



// TEST: click edge
// START ----------------------------------------------------------------------

// ACTION 1: repeat ACTION 1 above

// ACTION 2: click every edge in the graph, except inheritance
// RESULTS 2:
// - each edge is highlighted on click
// - graph layout does not change

// ACTION 3: Click into graph void
// RESULT 3: Edge is deactivated

// ACTION 4: click inheritance edge
// RESULTS 4:
// - graph layout changes
// - graph shows base and derived class node
// - inheritance edge is active

// END ------------------------------------------------------------------------



// TEST: move nodes
// START ----------------------------------------------------------------------

// ACTION 1: repeat ACTION 1 above

// ACTION 2: click and drag node Class by clicking on "Class"
// RESULTS 2:
// - node is moveable
// - on mouse release the node sticks to a grid

// ACTION 3: repeat ACTION 2 but click on "PUBLIC"
// ACTION 4: repeat ACTION 2 but click on "method"

// ACTION 5: click and drag node Class far to the left
// RESULTS 5:
// - all connected edges keep pointing correctly
// - the graph increases it's size to fit the full graph

// ACTION 6: repeat ACTION 5 to right
// ACTION 7: repeat ACTION 5 to top
// ACTION 8: repeat ACTION 5 to bottom

// END ------------------------------------------------------------------------



// TEST: hide nodes and edges
// START ----------------------------------------------------------------------

// ACTION 1: repeat ACTION 1 above

// ACTION 2: hide node 'Struct::Struct()' (context menu action or shortcut)
// RESULTS 2:
// - 'Struct::Struct()' is hidden
// - call edge to 'Struct::Struct()' is hidden

// ACTION 3: hide inheritance edge
// RESULT 3: inheritance edge is hidden

// ACTION 4: hide node 'Class'
// RESULTS 4:
// - 'Class' and all members are hidden
// - all edges to 'Class' are hidden

// ACTION 5: hide type use edge to 'TypeDef'
// RESULTS 5:
// - 'Typedef' is hidden
// - type use edge is hidden

// ACTION 6: hide node 'User'
// RESULT 6: nothing happens, error in status bar

// ACTION 7: hide node 'User::method()'
// RESULT 7: nothing happens, error in status bar

// END ------------------------------------------------------------------------



// TEST: bundle expand
// START ----------------------------------------------------------------------

namespace interaction
{
	struct BundleBase // <- ACTION 1: activate node
	{
		void bundles()
		{
			Class c; Struct s; Enum e = ENUM_CONSTANT; TypeDef d;
			CollapsedClass c1; ExpandedClass e1; User u;
			Str1 st1; Str2 st2; Str3 st3; Str4 st4; Str5 st5;

			e1.method1();
			e1.method2();
			e1.method3();
			e1.member;

			std::map<int, int> m;
			for (auto it = m.begin(); it != m.end(); it++)
				m.find(1);
		}
	};
}

// RESULT 1: graph shows 2 bundles

// ACTION 2: Click on 'Non-Indexed Symbols'
// RESULT 2: bundle is split

// ACTION 3: Click on edge going to 'Referenced Symbols'
// RESULT 3: bundle is split

// END ------------------------------------------------------------------------



// TEST: aggregation expand
// START ----------------------------------------------------------------------

// ACTION 1: Click on aggregation between 'BundleBase -> ExpandedClass'
// RESULTS 1:
// - graph shows 6 edges: 4 call, 1 use, 1 type use
// - edges are displayed as bezier curves

// ACTION 2: Click on any edge
// RESULT 2: Edge is activated and code shows it's location

// ACTION 3: Click into graph void
// RESULT 3: Edge is deactivated

// END ------------------------------------------------------------------------



// TEST: copy name
// START ----------------------------------------------------------------------

void copy_this_name(); // <- ACTION 1: Activate function

// ACTION 2: Use context menu action 'Copy Name' and paste to text editor
// RESULT 2: 'copy_this_name' is pasted

// END ------------------------------------------------------------------------



// TEST: file node actions
// START ----------------------------------------------------------------------

#include "interaction_files/file.h" // <- ACTION 1: Activate file

// ACTION 2: Context menu action 'Copy Full Path' on file node, paste to editor
// ACTION 2: Absolute file path is pasted

// ACTION 3: Context menu action 'Open Containing Folder' on file node
// ACTION 3: File explorer shows parent directory of file

// END ------------------------------------------------------------------------
