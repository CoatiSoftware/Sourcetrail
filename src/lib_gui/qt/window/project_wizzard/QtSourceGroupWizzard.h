#ifndef QT_SOURCE_GROUP_WIZZARD_H
#define QT_SOURCE_GROUP_WIZZARD_H

#include <functional>
#include <memory>
#include <vector>

#include "QtProjectWizzardContentGroup.h"
#include "QtProjectWizzardWindow.h"
#include "QtSourceGroupWizzardPage.h"
#include "logging.h"

class SourceGroupSettings;

class QtSourceGroupWizzardBase
{
public:
	virtual ~QtSourceGroupWizzardBase() = default;
	virtual void execute(QtWindowStack& windowStack) = 0;
	virtual bool canProcessSettings(std::shared_ptr<const SourceGroupSettings> settings) = 0;
};

template <typename SettingsType>
class QtSourceGroupWizzard: public QtSourceGroupWizzardBase
{
public:
	QtSourceGroupWizzard(
		std::shared_ptr<SettingsType> settings,
		std::function<void()> onCancelClicked,
		std::function<void(std::shared_ptr<SourceGroupSettings>)> onFinishedWizzard
	);

	void addPage(const QtSourceGroupWizzardPage<SettingsType>& page);
	void execute(QtWindowStack& windowStack) override;
	bool canProcessSettings(std::shared_ptr<const SourceGroupSettings> settings) override;

private:
	void createWindowForPage(const size_t pageId, QtWindowStack& windowStack);
	int mapToPageIdWithContentForContext(size_t pageId, WizzardContentContextType contextType) const;

	std::shared_ptr<SettingsType> m_settings;
	std::function<void()> m_onCancelClicked;
	std::function<void(std::shared_ptr<SourceGroupSettings>)> m_onFinishedWizzard;
	std::vector<QtSourceGroupWizzardPage<SettingsType>> m_pages;
};

template <typename SettingsType>
QtSourceGroupWizzard<SettingsType>::QtSourceGroupWizzard(
	std::shared_ptr<SettingsType> settings,
	std::function<void()> onCancelClicked,
	std::function<void(std::shared_ptr<SourceGroupSettings>)> onFinishedWizzard
)
	: m_settings(settings)
	, m_onCancelClicked(onCancelClicked)
	, m_onFinishedWizzard(onFinishedWizzard)
{
}

template <typename SettingsType>
void QtSourceGroupWizzard<SettingsType>::addPage(const QtSourceGroupWizzardPage<SettingsType>& page)
{
	m_pages.push_back(page);
}

template <typename SettingsType>
void QtSourceGroupWizzard<SettingsType>::execute(QtWindowStack& windowStack)
{
	if (!m_pages.empty())
	{
		createWindowForPage(mapToPageIdWithContentForContext(0, WIZZARD_CONTENT_CONTEXT_SETUP), windowStack);
	}
}

template <typename SettingsType>
bool QtSourceGroupWizzard<SettingsType>::canProcessSettings(std::shared_ptr<const SourceGroupSettings> settings)
{
	if (std::dynamic_pointer_cast<const SettingsType>(settings))
	{
		return true;
	}
	return false;
}

template <typename SettingsType>
void QtSourceGroupWizzard<SettingsType>::createWindowForPage(const size_t pageId, QtWindowStack& windowStack)
{
	if (pageId >= m_pages.size())
	{
		LOG_ERROR("Project Setup does not contain page " + std::to_string(pageId));
		return;
	}

	const QtSourceGroupWizzardPage<SettingsType>& page = m_pages.at(pageId);
	const int nextPageId = mapToPageIdWithContentForContext(pageId + 1, WIZZARD_CONTENT_CONTEXT_SETUP);

	QtProjectWizzardWindow* window = new QtProjectWizzardWindow(nullptr);

	window->connect(window, &QtProjectWizzardWindow::previous, &windowStack, &QtWindowStack::popWindow);
	window->connect(window, &QtProjectWizzardWindow::canceled, m_onCancelClicked);

	if (nextPageId > 0)
	{
		window->connect(
			window, &QtProjectWizzardWindow::next,
			[this, nextPageId, &windowStack]() { this->createWindowForPage(nextPageId, windowStack); }
		);
	}
	else
	{
		window->connect(window, &QtProjectWizzardWindow::next, [&]() { m_onFinishedWizzard(m_settings); });
	}

	QtProjectWizzardContentGroup* contentGroup = page.createContentGroup(WIZZARD_CONTENT_CONTEXT_SETUP, m_settings, window);

	window->setPreferredSize(QSize(page.getPreferredWidth(), page.getPreferredHeight()));
	window->setContent(contentGroup);
	window->setScrollAble(window->content()->isScrollAble());
	window->setup();

	size_t currentPage = 0;
	size_t totalPages = 0;
	for (size_t i = 0; i < m_pages.size(); i++)
	{
		if (m_pages[i].hasContentForContext(WIZZARD_CONTENT_CONTEXT_SETUP))
		{
			if (i <= pageId)
			{
				currentPage++;
			}
			totalPages++;
		}
	}

	window->updateSubTitle(QString::fromStdString(page.getTitle() + " - " + std::to_string(currentPage) + "/" + std::to_string(totalPages)));

	windowStack.pushWindow(window);
}

template <typename SettingsType>
int QtSourceGroupWizzard<SettingsType>::mapToPageIdWithContentForContext(size_t pageId, WizzardContentContextType contextType) const
{
	while (pageId < m_pages.size())
	{
		const QtSourceGroupWizzardPage<SettingsType>& page = m_pages.at(pageId);
		if (page.hasContentForContext(contextType))
		{
			return pageId;
		}
		pageId++;
	}
	return -1;
}

#endif // QT_SOURCE_GROUP_WIZZARD_H
