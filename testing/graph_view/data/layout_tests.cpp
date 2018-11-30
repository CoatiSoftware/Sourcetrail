
#define LAYOUT_TESTS


// TEST: call layout
// START ----------------------------------------------------------------------

void right_function()
{
}

void middle_function() // <- ACTION 1: activate function
{
	right_function();
}

void left_function()
{
	middle_function();
}

// RESULT 1: layout correctly displayed horizontally: left -> middle -> right

// END ------------------------------------------------------------------------



// TEST: inheritance layout
// START ----------------------------------------------------------------------

class Top {};

class Middle // <- ACTION 1: activate class
	: public Top
{};

class Bottom
	: public Middle
{};

// RESULT 1: layout correctly displayed vertically:
// - Top
// - Middle
// - Bottom

// END ------------------------------------------------------------------------



// TEST: override layout
// START ----------------------------------------------------------------------

class AbstractBase {
protected:
	virtual void foo() = 0;
	virtual void bar() = 0;
};

class Base : public AbstractBase {
protected:
	void foo() override {} // <- ACTION 1: activate
};

class Derived : public Base {
protected:
	void bar() override // <- ACTION 2: activate
	{
		foo();
	}
};

// RESULTS 1: layout correctly displayed vertically:
// - AbstractBase
// - Base
// - Derived
// override, call and inheritance edges visible

// RESULTS 2:
// - AbstractBase is connected with dashed inheritance edge

// ACTION 3: Click on dashed inheritance edge
// RESULT 3: inheritance chain is shown: AbstractBase -> Base -> Derived

// END ------------------------------------------------------------------------



// TEST: enum layout
// START ----------------------------------------------------------------------

enum Category // <- ACTION 1: activate
{
	ONE,
	TWO,
	THREE // <- ACTION 2: activate
};

// RESULTS 1:
// - enum shows all chilren
// - enum is expandable/collapsable
// - children layouted in nameless access frame

// RESULT 2: same as Action 1

// END ------------------------------------------------------------------------



// TEST: function children layout
// START ----------------------------------------------------------------------

void foobar() // <- ACTION 1: activate
{
	struct A {};
	struct B {}; // <- ACTION 2: activate
}

// RESULTS 1:
// - function shows all chilren
// - function is not expandable/collapsable
// - children layouted in nameless access frame

// RESULT 2: same as Action 1, but second struct is not visible

// END ------------------------------------------------------------------------



// TEST: implicit classes
// START ----------------------------------------------------------------------

template<typename T>
class BaseType // <- ACTION 1: activate
{
};

void implicit_type_creator() // <- ACTION 2: activate
{
	BaseType<int> a;
	BaseType<bool> b;
}

// RESULT 1: implicit classes shown in vertical layout

// RESULT 2: implicit classes shown as used types

// END ------------------------------------------------------------------------



// TEST: implicit methods
// START ----------------------------------------------------------------------

class AnotherType // <- ACTION 1: activate
{
public:
	template<typename T>
	void foo(T t) {} // <- ACTION 2: activate
};

void implicit_method_creator() // <- ACTION 3: activate
{
	AnotherType a;
	a.foo(1);
	a.foo(true);
}

// RESULT 1: class shows method, but not implicit methods

// RESULT 2: implicit methods appear

// RESULT 3: calls to implicit methods visible

// END ------------------------------------------------------------------------



// TEST: namespace layout
// START ----------------------------------------------------------------------

namespace layout // <- ACTION: activate
{
	int a;

	class B;

	typedef int C;
}

// RESULTS:
// - namespace displayed as group
// - all contents displayed
// - includes a letter index

// END ------------------------------------------------------------------------



// TEST: file contents
// START ----------------------------------------------------------------------

// ACTION: activate this file
// RESULTS:
// - file node contains all nodes defined in this file
// - file node is expanded
// - file node can be collapsed

// END ------------------------------------------------------------------------

