
int number = 3;

int calculate()
{
	int a = 1 + 2;
	return a;
}


int count = 0;

void countUp()
{
	count += 1;
}

void countDown()
{
	count -= 1;
}


void countUpTen()
{
	for (int i = 0; i < 10; i++)
	{
		countUp();
	}
}


class Person
{
public:
	Person(char* name)
		: name(name)
		, age(0)
		, weight(3)
		, height(50)
	{
	}

	char* askForName() const
	{
		return name;
	}

	void gainWeight(int gain)
	{
		weight += gain;
	}

	void grow(int gain)
	{
		height += gain;
	}

	void celebrateBirthday()
	{
		age += 1;

		grow(5);
		gainWeight(3);
	}

private:
	char* name;
	int age;

	int height;
	int weight;
};


class Vehicle
{
protected:
	int wheelCount;
	int seatCount;
};

class Car
	: public Vehicle
{
public:
	Car()
	{
		wheelCount = 4;
		seatCount = 5;
	}
};

class Truck
	: public Vehicle
{
public:
	Truck()
	{
		wheelCount = 6;
		seatCount = 2;
	}
};


template<typename T>
T sum(T a, T b)
{
	return a + b;
}

class Apple {};
class Pear {};

template<typename Fruit>
class Basket
{
public:
	Fruit** field;
};

Basket<Apple> apples;
Basket<Pear> pears;
