#ifndef PROPERTY_H
#define PROPERTY_H

template <class T>
class Property
{
public:
	explicit Property(T* valuePointer);
	~Property();

	T& operator=(const T& value);
	Property<T>& operator=(const Property<T>& property);

	operator const T&() const;
	T& operator()() const;

private:
	T* m_valuePointer;
};

template <class T>
Property<T>::Property(T* valuePointer): m_valuePointer(valuePointer)
{
}

template <class T>
Property<T>::~Property()
{
}

template <class T>
T& Property<T>::operator=(const T& value)
{
	*m_valuePointer = value;
	return *m_valuePointer;
}

template <class T>
Property<T>& Property<T>::operator=(const Property<T>& property)
{
	return *this;
}

template <class T>
Property<T>::operator const T&() const
{
	return *m_valuePointer;
}

template <class T>
T& Property<T>::operator()() const
{
	return *m_valuePointer;
}

#endif	  // PROPERTY_H
