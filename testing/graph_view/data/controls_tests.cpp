
#define CONTROLS_TESTS

namespace image_export
{
	struct Struct
	{
		static void foo() {};
	};

	class Base
	{
		virtual void foo() = 0;
	};

	class Center
		: public Base
	{
	public:
		void foo() // <- ACTION: activate foo()
		{
			Struct s;
			Struct::foo();
			member = 2;
		}
	protected:
		int member;
	};

	class Derived
		: public Center
	{
		void foo() {}
	};

	void bar()
	{
		Center c;
		c.foo();
	}
}


// TEST: move - drag
// START ----------------------------------------------------------------------

// ACTION: Click into graph void and drag
// RESULT: graph is moved

// END ------------------------------------------------------------------------



// TEST: move - Ctrl + Arrows
// START ----------------------------------------------------------------------

// ACTION: Use Ctrl + Arrows to scroll up, down, left, right
// RESULT: graph is moved

// END ------------------------------------------------------------------------



// TEST: zoom - buttons
// START ----------------------------------------------------------------------

// ACTION : Use screen zoom buttons bottom left
// RESULT: Zoom Level changes, percentage in lower left corner

// END ------------------------------------------------------------------------



// TEST: zoom - mouse wheel
// START ----------------------------------------------------------------------

// ACTION: Hold Shift and scroll in the graph
// RESULT: Zoom Level changes, percentage in lower left corner

// END ------------------------------------------------------------------------



// TEST: zoom - Shift + Ctrl + Up/Down
// START ----------------------------------------------------------------------

// ACTION: Hold Shift + Ctrl and press Up/Down
// RESULT: Zoom Level changes, percentage in lower left corner

// END ------------------------------------------------------------------------



// TEST: zoom reset
// START ----------------------------------------------------------------------

// ACTION: Press button '0'
// RESULT: Zoom level is reset

// END ------------------------------------------------------------------------



// TEST: image export
// START ----------------------------------------------------------------------

// ACTION: Export graph in every image format
// RESULT: compare to images in "results/controls_tests"

// END ------------------------------------------------------------------------



// TEST: legend
// START ----------------------------------------------------------------------

// ACTION 1: click '?' in lower right corner
// RESULT 1: graph legend is displayed

// ACTION 2: click on any node
// RESULT 2: nothing happens

// ACTION 3: go back and click any node
// RESULT 3: node is activated

// END ------------------------------------------------------------------------
