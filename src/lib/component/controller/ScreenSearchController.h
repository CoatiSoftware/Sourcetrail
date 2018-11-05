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
	virtual ~ScreenSearchController() = default;

	// Controller implementation
	void clear() override;

	// ScreenSearchSender implementation
	void foundMatches(ScreenSearchResponder* responder, size_t matchCount) override;
	void addResponder(ScreenSearchResponder* responder) override;
	void removeResponder(ScreenSearchResponder* responder) override;
	void clearMatches() override;

	void search(const std::wstring& query, const std::set<std::string>& responderNames);
	void activateMatch(bool next);

private:
	size_t getResponderId(ScreenSearchResponder* responder) const;

	std::vector<ScreenSearchResponder*> m_responders;

	std::vector<std::pair<size_t, size_t>> m_matches;
	size_t m_matchIndex = 0;
	std::mutex m_matchMutex;
};

#endif // SCREEN_SEARCH_CONTROLLER_H
