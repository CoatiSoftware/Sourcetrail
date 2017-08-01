#ifndef QT_THREADED_FUCTOR_H
#define QT_THREADED_FUCTOR_H

#include <chrono>
#include <functional>
#include <thread>

#include <QObject>
#include <QSemaphore>

class QtThreadedFunctorHelper: public QObject
{
	Q_OBJECT

signals:
	void signalExecution();

private slots:
	void execute()
	{
		m_callback();
		m_freeCallbacks.release();
	}

public:
	QtThreadedFunctorHelper()
		: m_freeCallbacks(1)
	{
		QObject::connect(this, &QtThreadedFunctorHelper::signalExecution, this, &QtThreadedFunctorHelper::execute);
	}

	void operator()(std::function<void(void)> callback)
	{
		m_freeCallbacks.acquire();
		m_callback = callback;
		signalExecution();
	}

private:
	std::function<void(void)> m_callback;
	QSemaphore m_freeCallbacks;
};

template <typename T1 = void, typename T2 = void, typename T3 = void, typename T4 = void>
class QtThreadedFunctor
{
public:
	QtThreadedFunctor(std::function<void(T1, T2, T3, T4)> callback) : m_callback(callback) {}

	void operator()(T1 p1, T2 p2, T3 p3, T4 p4)
	{
		m_helper(std::bind(m_callback, p1, p2, p3, p4));
	}

private:
	QtThreadedFunctorHelper m_helper;
	std::function<void(T1, T2, T3, T4)> m_callback;
};

template <typename T1, typename T2, typename T3>
class QtThreadedFunctor<T1, T2, T3, void>
{
public:
	QtThreadedFunctor(std::function<void(T1, T2, T3)> callback) : m_callback(callback) {}

	void operator()(T1 p1, T2 p2, T3 p3)
	{
		m_helper(std::bind(m_callback, p1, p2, p3));
	}

private:
	QtThreadedFunctorHelper m_helper;
	std::function<void(T1, T2, T3)> m_callback;
};

template <typename T1, typename T2>
class QtThreadedFunctor<T1, T2, void, void>
{
public:
	QtThreadedFunctor(std::function<void(T1, T2)> callback) : m_callback(callback) {}

	void operator()(T1 p1, T2 p2)
	{
		m_helper(std::bind(m_callback, p1, p2));
	}

private:
	QtThreadedFunctorHelper m_helper;
	std::function<void(T1, T2)> m_callback;
};

template <typename T1>
class QtThreadedFunctor<T1, void, void, void>
{
public:
	QtThreadedFunctor(std::function<void(T1)> callback) : m_callback(callback) {}

	void operator()(T1 p1)
	{
		m_helper(std::bind(m_callback, p1));
	}

private:
	QtThreadedFunctorHelper m_helper;
	std::function<void(T1)> m_callback;
};

template <>
class QtThreadedFunctor<void, void, void, void>
{
public:
	QtThreadedFunctor(std::function<void(void)> callback) : m_callback(callback) {}

	void operator()()
	{
		m_helper(m_callback);
	}

private:
	QtThreadedFunctorHelper m_helper;
	std::function<void(void)> m_callback;
};


class QtThreadedLambdaFunctor
{
public:
	void operator()(std::function<void(void)> callback)
	{
		m_helper(callback);
	}

private:
	QtThreadedFunctorHelper m_helper;
};

#endif // QT_THREADED_FUCTOR_H
