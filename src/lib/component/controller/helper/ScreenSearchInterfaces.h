#ifndef SCREEN_SEARCH_INTERFACES_H
#define SCREEN_SEARCH_INTERFACES_H

class ScreenSearchResponder;

class ScreenSearchSender
{
public:
	virtual ~ScreenSearchSender() {}

	virtual void foundMatches(ScreenSearchResponder* responder, size_t matchCount) = 0;
};

class ScreenSearchResponder
{
public:
	virtual ~ScreenSearchResponder() {}

	virtual std::string getName() const = 0;
	virtual bool isVisible() const = 0;

	virtual void findMatches(ScreenSearchSender* sender, const std::string& query) = 0;
	virtual void activateMatch(size_t matchIndex) = 0;
	virtual void deactivateMatch(size_t matchIndex) = 0;
	virtual void clearMatches() = 0;
};

#endif // SCREEN_SEARCH_INTERFACES_H
