#ifndef TABS_VIEW_H
#define TABS_VIEW_H

#include <vector>

#include "View.h"

struct SearchMatch;

class TabsView: public View
{
public:
	TabsView(ViewLayout* viewLayout);
	virtual ~TabsView() = default;

	virtual std::string getName() const;

	virtual void clear() = 0;

	virtual void openTab(bool showTab, const SearchMatch& match) = 0;
	virtual void closeTab() = 0;
	virtual void destroyTab(Id tabId) = 0;
	virtual void selectTab(bool next) = 0;
	virtual void updateTab(Id tabId, const std::vector<SearchMatch>& matches) = 0;
};

#endif	  // TABS_VIEW_H
