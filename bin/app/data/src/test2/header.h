template <typename T = int, typename U = int>
class TemplateTestClass
{
};

template<typename T>
class TemplateTestClass<int, T>
{
	int foo;
};

template<>
class TemplateTestClass<float, float>
{
	int foo;
};