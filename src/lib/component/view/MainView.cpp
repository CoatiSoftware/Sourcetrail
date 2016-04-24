#include "component/view/MainView.h"

MainView::MainView()
{
}

MainView::~MainView()
{
}

int MainView::confirm(const std::string& message)
{
	return confirm(message, std::vector<std::string>());
}
