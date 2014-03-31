#ifndef VIEW_MANAGER_H
#define VIEW_MANAGER_H

#include <memory>
#include <vector>

#include "component/view/View.h"

class ViewManager
{
public:
	ViewManager();
	~ViewManager();

private:
	std::vector<std::shared_ptr<View>> m_views;
};


#endif // VIEW_MANAGER_H
