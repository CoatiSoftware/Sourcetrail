#include "header.h"

char* name;
H g;

int ceil(float a)
{
	return static_cast<int>(a) + 1;
}

namespace X
{
	struct A;

	enum E
	{
		P,
		Q
	};
}

class B
{
public:
	class C
	{
		static const int amount;
	};

	B();

	virtual ~B()
	{
	}

protected:
	virtual void process() = 0;

private:
	int getCount() const
	{
		return count;
	}

	const int count;
	H h;
};

B::B()
	: count(0)
{
}

namespace
{
	int sum(int a, int b)
	{
		int c = a + b;
		return c;
	}
}
