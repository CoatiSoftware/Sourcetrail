const bool *ab(int abc, int bca);

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
	A()
		: m_importantValue(42)
		, m_importanterValue('r')
		, m_importantestValue(3.14159265359f)
	{
	}

	~A()
	{
	}

	void doImportantStuff()
	{
		m_importantValue *= 1;
	}

	void doModeratelyImportantStuff()
	{
		m_importanterValue = 'R';

		sum(21, 21);
	}

private:
	int m_importantValue;
	char m_importanterValue;
	float m_importantestValue;
};

A globalA;

class B : public A {};

class C
{
public:
	C()
		: m_valuable(0)
	{
		globalA.doImportantStuff();
	}

	~C()
	{
	}

	void solveAllProblems()
	{
		A aInstance;
		aInstance.doImportantStuff();
		aInstance.doModeratelyImportantStuff();
	}

private:
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
