#include "ScreenSearchController.h"

#include "../view/ScreenSearchView.h"

void ScreenSearchController::clear() {}

void ScreenSearchController::foundMatches(ScreenSearchResponder* responder, size_t matchCount)
{
	if (matchCount)
	{
		std::lock_guard<std::mutex> lock(m_matchMutex);

		size_t responderId = getResponderId(responder);
		if (!responderId)
		{
			return;
		}

		std::vector<std::pair<size_t, size_t>> newMatches;
		for (size_t i = 0; i < matchCount; i++)
		{
			newMatches.push_back(std::make_pair(responderId, i));
		}

		size_t i = 0;
		while (i < m_matches.size() && responderId > m_matches[i].first)
		{
			i++;
		}

		m_matches.insert(m_matches.begin() + i, newMatches.begin(), newMatches.end());
		m_matchIndex = m_matches.size();
	}

	getView<ScreenSearchView>()->setMatchCount(m_matches.size());
}

void ScreenSearchController::addResponder(ScreenSearchResponder* responder)
{
	if (responder)
	{
		m_responders.push_back(responder);
		getView<ScreenSearchView>()->addResponder(responder->getName());
	}
}

void ScreenSearchController::removeResponder(ScreenSearchResponder* responder)
{
	if (responder)
	{
		auto it = std::find(m_responders.begin(), m_responders.end(), responder);
		if (it != m_responders.end())
		{
			m_responders.erase(it);
		}
	}
}

void ScreenSearchController::search(const std::wstring& query, const std::set<std::string>& responderNames)
{
	clearMatches();

	for (ScreenSearchResponder* responder: m_responders)
	{
		if (!responder->isVisible())
		{
			continue;
		}

		if (query.size() && responderNames.find(responder->getName()) != responderNames.end())
		{
			responder->findMatches(this, query);
		}
	}
}

void ScreenSearchController::activateMatch(bool next)
{
	std::lock_guard<std::mutex> lock(m_matchMutex);
	if (!m_matches.size())
	{
		return;
	}

	if (m_matchIndex != m_matches.size())
	{
		auto match = m_matches[m_matchIndex];
		m_responders[match.first - 1]->deactivateMatch(match.second);
	}

	if (next)
	{
		if (m_matchIndex == m_matches.size())
		{
			m_matchIndex = 0;
		}
		else
		{
			m_matchIndex = (m_matchIndex + 1) % m_matches.size();
		}
	}
	else
	{
		if (m_matchIndex == 0)
		{
			m_matchIndex = m_matches.size() - 1;
		}
		else
		{
			m_matchIndex--;
		}
	}

	auto match = m_matches[m_matchIndex];
	m_responders[match.first - 1]->activateMatch(match.second);

	getView<ScreenSearchView>()->setMatchIndex(m_matchIndex + 1);
}

void ScreenSearchController::clearMatches()
{
	{
		std::lock_guard<std::mutex> lock(m_matchMutex);
		m_matches.clear();
		m_matchIndex = 0;
	}

	getView<ScreenSearchView>()->setMatchCount(0);

	for (ScreenSearchResponder* responder: m_responders)
	{
		responder->clearMatches();
	}
}

size_t ScreenSearchController::getResponderId(ScreenSearchResponder* responder) const
{
	for (size_t i = 0; i < m_responders.size(); i++)
	{
		if (m_responders[i] == responder)
		{
			return i + 1;
		}
	}

	return 0;
}

void ScreenSearchController::handleActivation(const MessageActivateBase* message)
{
	clearMatches();
}

void ScreenSearchController::handleMessage(MessageActivateLocalSymbols* message)
{
	clearMatches();
}

void ScreenSearchController::handleMessage(MessageActivateTrailEdge* message)
{
	clearMatches();
}

void ScreenSearchController::handleMessage(MessageChangeFileView* message)
{
	clearMatches();
}

void ScreenSearchController::handleMessage(MessageCodeShowDefinition* message)
{
	clearMatches();
}

void ScreenSearchController::handleMessage(MessageDeactivateEdge* message)
{
	clearMatches();
}

void ScreenSearchController::handleMessage(MessageGraphNodeBundleSplit* message)
{
	clearMatches();
}

void ScreenSearchController::handleMessage(MessageGraphNodeExpand* message)
{
	clearMatches();
}

void ScreenSearchController::handleMessage(MessageGraphNodeHide* message)
{
	clearMatches();
}
