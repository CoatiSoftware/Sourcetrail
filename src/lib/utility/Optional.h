#ifndef OPTIONAL_H
#define OPTIONAL_H

template <typename T>
class Optional
{
public:
	Optional();
	Optional(const T& value);

	T& get();
	const T& get() const;

	bool isPresent() const;

private:
	bool m_isPresent;
	T m_value;
};


template <typename T>
Optional<T>::Optional(): m_isPresent(false)
{
}

template <typename T>
Optional<T>::Optional(const T& value): m_isPresent(false), m_value(value)
{
}

template <typename T>
T& Optional<T>::get()
{
	return m_value;
}

template <typename T>
const T& Optional<T>::get() const
{
	return m_value;
}

template <typename T>
bool Optional<T>::isPresent() const
{
	return m_isPresent;
}

#endif	  // OPTIONAL_H
