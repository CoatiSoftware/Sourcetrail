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

noname::V voo;

typedef A* D;

D globalD;

class E
{
public:
	E(){}
	~E(){}

	class SubE
	{
	public:
		SubE(){}
		~SubE(){}
	};

private:
	int m_importantInt;
};

class AnotherClass
	: public A
{
public:
	AnotherClass()
		: A('x')
	{
	}

	int publicInt;
protected:
	int protectedInt;
private:
	int privateInt;
};


class circleA
{
public:
	circleA()
	{
		
	}

private:
	//circleB m_b;
};

class circleB
{
public:
	circleB()
	{
		m_a = circleA();
	}
	
	circleA foo(){return circleA();}

private:
	circleA m_a;
};

struct circleC
{
public:
	circleC()
	{
		m_b = circleB();
	}
	
	circleA foo0(){return circleA();}
	circleB foo1(){return circleB();}

private:
	circleB m_b;
};

class circleD
{
public:
	circleD()
	{
		m_c = circleC();
	}
	
	circleC foo(){return circleC();}
	
	circleA foo2(){return circleA();}
	circleB foo3(){return circleB();}

private:
	circleC m_c;
};

class circleE
{
public:
	circleE()
	{
		m_d = circleD();
	}
	
	circleC foo(){return circleC();}
	circleB foo1(){return circleB();}
	circleD foo2(){return circleD();}
	circleA foo3(){return circleA();}

private:
	circleD m_d;
};

class circleF
{
public:
	circleF()
	{
		m_e = circleE();
	}
	
	circleC foo(){return circleC();}
	circleB foo1(){return circleB();}
	circleD foo2(){return circleD();}
	circleA foo3(){return circleA();}
	circleE foo4(){return circleE();}

private:
	circleE m_e;
};

class circleCenter
{
public:
	circleCenter()
	{
		m_a = circleA();
		m_b = circleB();
		m_c = circleC();
		m_d = circleD();
	}
	
	circleA foo0(){return circleA();}
	circleB foo1(){return circleB();}
	circleC foo2(){return circleC();}
	circleD foo3(){return circleD();}
	circleE foo4(){return circleE();}
	circleF foo5(){return circleF();}
	
private:
	circleA m_a;
	circleB m_b;
	circleC m_c;
	circleD m_d;
	
	//AnotherClass m_anotherClass;
};