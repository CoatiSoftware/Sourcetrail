#ifndef TRACING_H
#define TRACING_H

#include <mutex>
#include <stack>
#include <thread>

#include "utility/TimeStamp.h"
#include "utility/types.h"

struct TraceEvent
{
public:
	TraceEvent(const std::string& eventName, Id id, size_t depth)
		: eventName(eventName)
		, id(id)
		, depth(depth)
		, time(0.0f)
	{
	}

	const std::string eventName;
	const Id id;
	const size_t depth;

	std::string functionName;
	std::string locationName;

	float time;
};


class Tracer
{
public:
	static Tracer* getInstance();

	TraceEvent* startEvent(const std::string& eventName);
	void finishEvent(TraceEvent* event);

	void printTraces();

private:
	static std::shared_ptr<Tracer> s_instance;
	static Id s_nextTraceId;

	Tracer();
	Tracer(const Tracer&);
	void operator=(const Tracer&);

	std::map<std::thread::id, std::vector<std::shared_ptr<TraceEvent>>> m_events;
	std::map<std::thread::id, std::stack<TraceEvent*>> m_startedEvents;

	std::mutex m_mutex;
};


class ScopedTrace
{
public:
	ScopedTrace(const std::string& eventName, const std::string& fileName, int lineNumber, const std::string& functionName);
	~ScopedTrace();

private:
	TraceEvent* m_event;
	TimeStamp m_TimeStamp;
};


// #define TRACING_ENABLED

#ifdef TRACING_ENABLED
	#define TRACE(__name__) \
		ScopedTrace __trace__(std::string(__name__), __FILE__, __LINE__, __FUNCTION__)

	#define PRINT_TRACES() \
		Tracer::getInstance()->printTraces()
#else
	#define TRACE(__name__)
	#define PRINT_TRACES()
#endif

#endif // TRACING_H
