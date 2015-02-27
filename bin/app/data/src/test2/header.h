template <typename T, typename U>
class TemplateTestClass
{
public:
	template <typename P>
	void run(P param);
};

template <typename T, typename U>
template <typename P>
void TemplateTestClass<T, U>::run(P param)
{
}

