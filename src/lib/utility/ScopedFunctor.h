#ifndef SCOPED_FUNCTOR_H
#define SCOPED_FUNCTOR_H

#include <functional>

class ScopedFunctor
{
public:
	ScopedFunctor();
	ScopedFunctor(std::function<void(void)> onDestroy);
	~ScopedFunctor();

private:
	std::function<void(void)> m_onDestroy;
};

#endif // SCOPED_FUNCTOR_H
