#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

const bool *abd(int abc, int bca);

bool const *abc(int a, int b);

bool *const abcd(int a, int b);

int main();
void foo();
int sum(int a, int b);
int diff(int a, int b);

void funk();

class A
{
public:
	A(char c)
		: m_importantValue(42)
		, m_importanterValue(c)
		, m_importantestValue(3.14159265359f)
	{
	}

	~A()
	{
	}

	void doImportantStuff()
	{
		int a = 1;
		m_importantValue *= a;
	}

	void doModeratelyImportantStuff()
	{
		m_importanterValue = 'R';
		int answer = sum(21, 21);
	}

private:
	int m_importantValue;
	char m_importanterValue;
	const float m_importantestValue;
};

A globalA(' ');

class B : public A
{
public:
	B()
		: A('b')
	{
	}

	virtual ~B()
	{
	}

protected:
	virtual float getNumber() = 0;
};

class C
{
public:
	C()
		: m_valuable(0)
	{
		globalA.doImportantStuff();
		s_count++;
	}

	~C()
	{
		s_count--;
	}

	static int getCount()
	{
		return s_count;
	}

	void solveAllProblems() const
	{
		A aInstance('a');
		aInstance.doImportantStuff();
		aInstance.doModeratelyImportantStuff();
	}

private:
	static int s_count;
	int m_valuable;
};

namespace
{
	namespace noname
	{
		struct V
		{
			int x;
			int y;
		};
	}

	enum E
	{
		X,
		Y
	};
}

typedef A* D;

D globalD;
