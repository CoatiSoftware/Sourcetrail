#ifndef QT_SOURCE_GROUP_WIZARD_PAGE_H
#define QT_SOURCE_GROUP_WIZARD_PAGE_H

#include <functional>
#include <memory>
#include <vector>

class QtProjectWizardWindow;

enum WizardContentContextType
{
	WIZARD_CONTENT_CONTEXT_SETUP = 1,
	WIZARD_CONTENT_CONTEXT_SUMMARY = 2,
	WIZARD_CONTENT_CONTEXT_ALL = 3
};

template <typename SettingsType>
class QtSourceGroupWizardPage
{
public:
	typedef std::function<QtProjectWizardContent* (std::shared_ptr<SettingsType> settings, QtProjectWizardWindow* window)> ContentCreator;

	QtSourceGroupWizardPage(const std::string& title, int preferredWidth = 750, int preferredHeight = 600);

	void addContentCreator(
		WizardContentContextType contextType,
		ContentCreator contentCreator
	);

	template <typename ContentType>
	void addContentCreatorSimple(WizardContentContextType contextType);

	template <typename ContentType>
	void addContentCreatorWithSettings(WizardContentContextType contextType);

	template <typename ContentType, typename ... ParamTypes>
	void addContentCreatorWithSettings(WizardContentContextType contextType, ParamTypes ... params);

	bool hasContentForContext(WizardContentContextType contextType) const;

	std::string getTitle() const;
	int getPreferredWidth() const;
	int getPreferredHeight() const;
	QtProjectWizardContentGroup* createContentGroup(WizardContentContextType contextType, std::shared_ptr<SettingsType> settings, QtProjectWizardWindow* window) const;

private:
	const std::string m_title;
	const int m_preferredWidth;
	const int m_preferredHeight;
	std::vector<std::pair<WizardContentContextType, ContentCreator>> m_contentCreators;
};

template <typename SettingsType>
QtSourceGroupWizardPage<SettingsType>::QtSourceGroupWizardPage(const std::string& title, int preferredWidth, int preferredHeight)
	: m_title(title)
	, m_preferredWidth(preferredWidth)
	, m_preferredHeight(preferredHeight)
{
}

template <typename SettingsType>
void QtSourceGroupWizardPage<SettingsType>::addContentCreator(
	WizardContentContextType contextType,
	ContentCreator contentCreator
)
{
	m_contentCreators.push_back(std::make_pair(contextType, contentCreator));
}

template <typename SettingsType>
template <typename ContentType>
void QtSourceGroupWizardPage<SettingsType>::addContentCreatorSimple(WizardContentContextType contextType)
{
	addContentCreator(
		contextType,
		[](std::shared_ptr<SettingsType> settings, QtProjectWizardWindow* window)
		{
			return new ContentType(window);
		}
	);
}

template <typename SettingsType>
template <typename ContentType>
void QtSourceGroupWizardPage<SettingsType>::addContentCreatorWithSettings(WizardContentContextType contextType)
{
	addContentCreator(
		contextType,
		[](std::shared_ptr<SettingsType> settings, QtProjectWizardWindow* window)
		{
			return new ContentType(settings, window);
		}
	);
}

template <typename SettingsType>
template <typename ContentType, typename ... ParamTypes>
void QtSourceGroupWizardPage<SettingsType>::addContentCreatorWithSettings(WizardContentContextType contextType, ParamTypes ... params)
{
	addContentCreator(
		contextType,
		[=](std::shared_ptr<SettingsType> settings, QtProjectWizardWindow* window)
		{
			return new ContentType(settings, window, params...);
		}
	);
}

template <typename SettingsType>
std::string QtSourceGroupWizardPage<SettingsType>::getTitle() const
{
	return m_title;
}

template <typename SettingsType>
int QtSourceGroupWizardPage<SettingsType>::getPreferredWidth() const
{
	return m_preferredWidth;
}

template <typename SettingsType>
int QtSourceGroupWizardPage<SettingsType>::getPreferredHeight() const
{
	return m_preferredHeight;
}

template <typename SettingsType>
bool QtSourceGroupWizardPage<SettingsType>::hasContentForContext(WizardContentContextType contextType) const
{
	for (const std::pair<WizardContentContextType, ContentCreator>& contentCreator : m_contentCreators)
	{
		if (contentCreator.first & contextType)
		{
			return true;
		}
	}
	return false;
}

template <typename SettingsType>
QtProjectWizardContentGroup* QtSourceGroupWizardPage<SettingsType>::createContentGroup(WizardContentContextType contextType, std::shared_ptr<SettingsType> settings, QtProjectWizardWindow* window) const
{
	QtProjectWizardContentGroup* contentGroup = new QtProjectWizardContentGroup(window);

	bool firstContentAdded = false;
	for (const std::pair<WizardContentContextType, ContentCreator>& contentCreator : m_contentCreators)
	{
		if (contentCreator.first & contextType)
		{
			if (firstContentAdded)
			{
				contentGroup->addSpace();
			}
			contentGroup->addContent(contentCreator.second(settings, window));
			firstContentAdded = true;
		}
	}

	return contentGroup;
}


#endif // QT_SOURCE_GROUP_WIZARD_PAGE_H
