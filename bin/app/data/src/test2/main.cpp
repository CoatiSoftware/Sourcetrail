#include "header.h"

#include <vector>

int main()
{
	std::vector<Fooo<int>> yay;
	
	for (Fooo<int> a: yay)
	{
		a;
	}
	
	Foo<true, int, float> ft;
	ft.bar();
	return 0;

}




