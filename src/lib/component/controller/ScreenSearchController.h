#ifndef SCREEN_SEARCH_CONTROLLER_H
#define SCREEN_SEARCH_CONTROLLER_H

#include <mutex>
#include <set>

#include "Controller.h"
#include "ScreenSearchInterfaces.h"
#include "MessageListener.h"

class ScreenSearchController
	: public Controller
	, public ScreenSearchSender
{
public:
	ScreenSearchController();
	virtual ~ScreenSearchController();

	// Controller implementation
	virtual void clear();

	// ScreenSearchSender implementation
	virtual void foundMatches(ScreenSearchResponder* responder, size_t matchCount);

	void addResponder(ScreenSearchResponder* responder);
	void search(const std::wstring& query, const std::set<std::string>& responderNames);
	void activateMatch(bool next);
	void clearMatches();

private:
	size_t getResponderId(ScreenSearchResponder* responder) const;

	std::vector<ScreenSearchResponder*> m_responders;

	std::vector<std::pair<size_t, size_t>> m_matches;
	size_t m_matchIndex = 0;
	std::mutex m_matchMutex;
};

#endif // SCREEN_SEARCH_CONTROLLER_H
