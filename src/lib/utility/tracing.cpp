#include "utility/tracing.h"

#include "utility/utility.h"

std::shared_ptr<Tracer> Tracer::s_instance;
Id Tracer::s_nextTraceId = 0;

Tracer* Tracer::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<Tracer>(new Tracer());
	}

	return s_instance.get();
}

TraceEvent* Tracer::startEvent(const std::string& eventName)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	std::thread::id id = std::this_thread::get_id();
	std::shared_ptr<TraceEvent> event =
		std::make_shared<TraceEvent>(eventName, s_nextTraceId++, m_startedEvents[id].size());

	m_events[id].push_back(event);
	m_startedEvents[id].push(event.get());

	return event.get();
}

void Tracer::finishEvent(TraceEvent* event)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	std::thread::id id = std::this_thread::get_id();
	m_startedEvents[id].pop();
}

void Tracer::printTraces()
{
	std::lock_guard<std::mutex> lock(m_mutex);

	size_t unfinishEvents = 0;
	for (auto& p : m_startedEvents)
	{
		unfinishEvents += p.second.size();
	}

	if (unfinishEvents > 0)
	{
		std::cout << "TRACING: Trace events are still running." << std::endl;
		return;
	}
	else if (!m_events.size())
	{
		std::cout << "TRACING: No trace events collected." << std::endl;
		return;
	}


	std::cout << "TRACING\n--------------------------\n" << std::endl;

	std::cout << "HISTORY:\n\n";
	std::cout << "    time                 name                     function";
	std::cout << "                                          location\n";
	std::cout << "-----------------------------------------------------------------";
	std::cout << "------------------------------------------------------------\n";

	for (auto& p : m_events)
	{
		std::cout << "thread: " << p.first << std::endl;

		for (const std::shared_ptr<TraceEvent>& event : p.second)
		{
			std::cout.width(8 + 2 * event->depth);
			std::cout << std::right << std::setprecision(3) << std::fixed << event->time;

			std::cout.width(17 - 2 * event->depth);
			std::cout << " ";

			std::cout.width(25);
			std::cout << std::left << event->eventName;

			std::cout.width(50);
			std::cout << (event->functionName + "()") << event->locationName << std::endl;
		}

		std::cout << std::endl;
	}

	std::cout << "\nREPORT:\n\n";
	std::cout << "    time      count      name                     function";
	std::cout << "                                          location\n";
	std::cout << "-----------------------------------------------------------------";
	std::cout << "------------------------------------------------------------\n";

	struct AccumulatedTraceEvent
	{
		TraceEvent* event;
		size_t count;
		float time;
	};

	std::map<std::string, AccumulatedTraceEvent> accumulatedEvents;

	for (auto& p : m_events)
	{
		for (const std::shared_ptr<TraceEvent>& event : p.second)
		{
			std::string name = event->eventName + event->functionName + event->locationName;

			std::pair<std::map<std::string, AccumulatedTraceEvent>::iterator, bool> p =
				accumulatedEvents.emplace(name, AccumulatedTraceEvent());

			AccumulatedTraceEvent* acc = &p.first->second;
			if (p.second)
			{
				acc->event = event.get();
				acc->time = event->time;
				acc->count = 1;
			}
			else
			{
				acc->time += event->time;
				acc->count++;
			}
		}
	}

	std::multiset<AccumulatedTraceEvent,
			std::function<bool (const AccumulatedTraceEvent&, const AccumulatedTraceEvent&)>> sortedEvents(
		[](const AccumulatedTraceEvent& a, const AccumulatedTraceEvent& b)
		{
			return a.time > b.time;
		}
	);

	for (const std::pair<std::string, AccumulatedTraceEvent>& p : accumulatedEvents)
	{
		sortedEvents.insert(p.second);
	}

	for (const AccumulatedTraceEvent& acc : sortedEvents)
	{
		std::cout.width(8);
		std::cout << std::right << std::setprecision(3) << std::fixed << acc.time;

		std::cout.width(10);
		std::cout << acc.count << "       ";

		std::cout.width(25);
		std::cout << std::left << acc.event->eventName;

		std::cout.width(50);
		std::cout << (acc.event->functionName + "()") << acc.event->locationName << std::endl;
	}

	std::cout << std::endl;

	m_events.clear();
}

Tracer::Tracer()
{
}


ScopedTrace::ScopedTrace(
	const std::string& eventName, const std::string& fileName, int lineNumber, const std::string& functionName)
{
	m_event = Tracer::getInstance()->startEvent(eventName);
	m_event->functionName = functionName;
	m_event->locationName = FilePath(fileName).fileName() + ":" + std::to_string(lineNumber);

	m_TimeStamp = utility::durationStart();
}

ScopedTrace::~ScopedTrace()
{
	m_event->time = utility::duration(m_TimeStamp);
	Tracer::getInstance()->finishEvent(m_event);
}
