#include <memory>

class A
{
};

template <typename T>
class B
{
};

int main()
{
	A a;

	A* aPtr = nullptr;

	std::shared_ptr<A> aSharedPtr = std::make_shared<A>();

	B<A> b;

	return 0;
}
