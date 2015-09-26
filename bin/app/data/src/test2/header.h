template <bool B, typename, class F>
class Foo
{
public:
	void bar()
	{
	}
};

template <typename T>
class Fooo
{
public:
	void bar()
	{
	}
};

template <typename T, typename T::a::type U>
class Foooo1
{
public:
	void bar()
	{
	}
};

template <typename T, typename T::type U>
class Foooo2
{
public:
	void bar()
	{
	}
};
