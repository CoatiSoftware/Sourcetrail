void func()
{
	int m;
}

void func2()
{
	func();
}

class CustomA
{
public:
	void method()
	{
		func();
	}

	int i;
};

class CustomB
{
public:
	void method()
	{
		a.method();
	}

	CustomA a;
};

class CustomC
{
public:
	int method()
	{
		func2();
		bool a;
		char c;
	}

	CustomA a;
};

class CustomD
{
public:
	void method()
	{
		b.method();
	}

	CustomB b;
	CustomC c;
};
