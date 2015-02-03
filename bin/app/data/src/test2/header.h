template <typename T, typename U>
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