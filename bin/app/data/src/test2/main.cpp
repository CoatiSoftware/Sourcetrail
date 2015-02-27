#include "header.h"

int main()
{
	TemplateTestClass<float, float> t1;
	t1.run<float>(6.9f);
	TemplateTestClass<int, int> t2;
	t2.run<int>(6);
	return 0;
}