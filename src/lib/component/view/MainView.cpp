#include "MainView.h"

#include "TabId.h"

MainView::MainView(const ViewFactory* viewFactory, StorageAccess* storageAccess)
	: m_componentManager(viewFactory, storageAccess)
{
}

void MainView::setup()
{
	m_componentManager.setupMain(this, TabId::app());

	loadLayout();

	m_componentManager.refreshViews();
}

void MainView::clear()
{
	m_componentManager.clearComponents();
	clearHistoryMenu();
	clearBookmarksMenu();
}

void MainView::refreshViews()
{
	refreshView();
	m_componentManager.refreshViews();
}

std::shared_ptr<DialogView> MainView::getDialogView(DialogView::UseCase useCase)
{
	return m_componentManager.getDialogView(useCase);
}
