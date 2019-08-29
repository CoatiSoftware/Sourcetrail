#include "ActivationListener.h"

const std::vector<SearchMatch>& ActivationListener::getSearchMatches() const
{
	return m_searchMatches;
}

void ActivationListener::handleMessage(MessageActivateErrors* message)
{
	handleMessageBase(message);
}

void ActivationListener::handleMessage(MessageActivateFullTextSearch* message)
{
	handleMessageBase(message);
}

void ActivationListener::handleMessage(MessageActivateLegend* message)
{
	handleMessageBase(message);
}

void ActivationListener::handleMessage(MessageActivateOverview* message)
{
	handleMessageBase(message);
}

void ActivationListener::handleMessage(MessageActivateTokens* message)
{
	handleMessageBase(message);
}

void ActivationListener::handleMessage(MessageActivateTrail* message)
{
	handleMessageBase(message);
}

void ActivationListener::handleMessageBase(const MessageActivateBase* message)
{
	m_searchMatches = message->getSearchMatches();
	handleActivation(message);
	handleActivation(m_searchMatches);
}

void ActivationListener::handleActivation(const MessageActivateBase* message)
{
}

void ActivationListener::handleActivation(const std::vector<SearchMatch>& searchMatches)
{
}
