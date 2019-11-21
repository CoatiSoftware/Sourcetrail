#ifndef QT_SOURCE_GROUP_WIZARD_H
#define QT_SOURCE_GROUP_WIZARD_H

#include <functional>
#include <memory>
#include <vector>

#include "QtProjectWizardContentGroup.h"
#include "QtProjectWizardWindow.h"
#include "QtSourceGroupWizardPage.h"
#include "logging.h"

class SourceGroupSettings;

class QtSourceGroupWizardBase
{
public:
	virtual ~QtSourceGroupWizardBase() = default;
	virtual void execute(QtWindowStack& windowStack) = 0;
	virtual bool canProcessSettings(std::shared_ptr<const SourceGroupSettings> settings) = 0;
};

template <typename SettingsType>
class QtSourceGroupWizard: public QtSourceGroupWizardBase
{
public:
	QtSourceGroupWizard(
		std::shared_ptr<SettingsType> settings,
		std::function<void()> onCancelClicked,
		std::function<void(std::shared_ptr<SourceGroupSettings>)> onFinishedWizard);

	void addPage(const QtSourceGroupWizardPage<SettingsType>& page);
	void execute(QtWindowStack& windowStack) override;
	bool canProcessSettings(std::shared_ptr<const SourceGroupSettings> settings) override;

private:
	void createWindowForPage(const size_t pageId, QtWindowStack& windowStack);

	std::shared_ptr<SettingsType> m_settings;
	std::function<void()> m_onCancelClicked;
	std::function<void(std::shared_ptr<SourceGroupSettings>)> m_onFinishedWizard;
	std::vector<QtSourceGroupWizardPage<SettingsType>> m_pages;
};

template <typename SettingsType>
QtSourceGroupWizard<SettingsType>::QtSourceGroupWizard(
	std::shared_ptr<SettingsType> settings,
	std::function<void()> onCancelClicked,
	std::function<void(std::shared_ptr<SourceGroupSettings>)> onFinishedWizard)
	: m_settings(settings), m_onCancelClicked(onCancelClicked), m_onFinishedWizard(onFinishedWizard)
{
}

template <typename SettingsType>
void QtSourceGroupWizard<SettingsType>::addPage(const QtSourceGroupWizardPage<SettingsType>& page)
{
	m_pages.push_back(page);
}

template <typename SettingsType>
void QtSourceGroupWizard<SettingsType>::execute(QtWindowStack& windowStack)
{
	if (!m_pages.empty())
	{
		createWindowForPage(0, windowStack);
	}
}

template <typename SettingsType>
bool QtSourceGroupWizard<SettingsType>::canProcessSettings(
	std::shared_ptr<const SourceGroupSettings> settings)
{
	if (std::dynamic_pointer_cast<const SettingsType>(settings))
	{
		return true;
	}
	return false;
}

template <typename SettingsType>
void QtSourceGroupWizard<SettingsType>::createWindowForPage(const size_t pageId, QtWindowStack& windowStack)
{
	if (pageId >= m_pages.size())
	{
		LOG_ERROR("Project Setup does not contain page " + std::to_string(pageId));
		return;
	}

	const QtSourceGroupWizardPage<SettingsType>& page = m_pages.at(pageId);
	const int nextPageId = pageId + 1;

	QtProjectWizardWindow* window = new QtProjectWizardWindow(nullptr);

	window->connect(
		window, &QtProjectWizardWindow::previous, &windowStack, &QtWindowStack::popWindow);
	window->connect(window, &QtProjectWizardWindow::canceled, m_onCancelClicked);

	if (nextPageId > 0)
	{
		window->connect(window, &QtProjectWizardWindow::next, [this, nextPageId, &windowStack]() {
			this->createWindowForPage(nextPageId, windowStack);
		});
	}
	else
	{
		window->connect(
			window, &QtProjectWizardWindow::next, [&]() { m_onFinishedWizard(m_settings); });
	}

	QtProjectWizardContentGroup* contentGroup = page.createContentGroup(m_settings, window);

	window->setPreferredSize(QSize(page.getPreferredWidth(), page.getPreferredHeight()));
	window->setContent(contentGroup);
	window->setScrollAble(window->content()->isScrollAble());
	window->setup();

	size_t currentPage = 0;
	size_t totalPages = 0;
	for (size_t i = 0; i < m_pages.size(); i++)
	{
		if (i <= pageId)
		{
			currentPage++;
		}
		totalPages++;
	}

	window->updateSubTitle(QString::fromStdString(
		page.getTitle() + " - " + std::to_string(currentPage) + "/" + std::to_string(totalPages)));

	windowStack.pushWindow(window);
}

#endif	  // QT_SOURCE_GROUP_WIZARD_H
