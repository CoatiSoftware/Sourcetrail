#ifndef TOOLTIP_VIEW_H
#define TOOLTIP_VIEW_H

#include <vector>

#include "NameHierarchy.h"
#include "TooltipInfo.h"
#include "View.h"
#include "types.h"

class TooltipView: public View
{
public:
	TooltipView(ViewLayout* viewLayout);
	virtual ~TooltipView();

	// View implementation
	virtual std::string getName() const;

	virtual void showTooltip(const TooltipInfo& info, const View* parent) = 0;
	virtual void hideTooltip(bool force) = 0;

	virtual bool tooltipVisible() const = 0;
};

#endif	  // TOOLTIP_VIEW_H
