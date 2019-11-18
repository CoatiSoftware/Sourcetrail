#ifndef MESSAGE_ACTIVATE_BASE_H
#define MESSAGE_ACTIVATE_BASE_H

#include "SearchMatch.h"

class MessageActivateBase
{
public:
	virtual ~MessageActivateBase() = default;

	virtual std::vector<SearchMatch> getSearchMatches() const = 0;
};

#endif	  // MESSAGE_ACTIVATE_BASE_H
