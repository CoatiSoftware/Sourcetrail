#ifndef QT_SOURCE_GROUP_WIZARD_PAGE_H
#define QT_SOURCE_GROUP_WIZARD_PAGE_H

#include <functional>
#include <memory>
#include <vector>

class QtProjectWizardWindow;

template <typename SettingsType>
class QtSourceGroupWizardPage
{
public:
	typedef std::function<QtProjectWizardContent*(
		std::shared_ptr<SettingsType> settings, QtProjectWizardWindow* window)>
		ContentCreator;

	QtSourceGroupWizardPage(
		const std::string& title, int preferredWidth = 750, int preferredHeight = 600);

	void addContentCreator(ContentCreator contentCreator);

	template <typename ContentType>
	void addContentCreatorSimple();

	template <typename ContentType>
	void addContentCreatorWithSettings();

	template <typename ContentType, typename... ParamTypes>
	void addContentCreatorWithSettings(ParamTypes... params);

	std::string getTitle() const;
	int getPreferredWidth() const;
	int getPreferredHeight() const;
	QtProjectWizardContentGroup* createContentGroup(
		std::shared_ptr<SettingsType> settings,
		QtProjectWizardWindow* window) const;

private:
	const std::string m_title;
	const int m_preferredWidth;
	const int m_preferredHeight;
	std::vector<ContentCreator> m_contentCreators;
};

template <typename SettingsType>
QtSourceGroupWizardPage<SettingsType>::QtSourceGroupWizardPage(
	const std::string& title, int preferredWidth, int preferredHeight)
	: m_title(title), m_preferredWidth(preferredWidth), m_preferredHeight(preferredHeight)
{
}

template <typename SettingsType>
void QtSourceGroupWizardPage<SettingsType>::addContentCreator(ContentCreator contentCreator)
{
	m_contentCreators.push_back(contentCreator);
}

template <typename SettingsType>
template <typename ContentType>
void QtSourceGroupWizardPage<SettingsType>::addContentCreatorSimple()
{
	addContentCreator(
		[](std::shared_ptr<SettingsType> settings, QtProjectWizardWindow* window) {
			return new ContentType(window);
		});
}

template <typename SettingsType>
template <typename ContentType>
void QtSourceGroupWizardPage<SettingsType>::addContentCreatorWithSettings()
{
	addContentCreator(
		[](std::shared_ptr<SettingsType> settings, QtProjectWizardWindow* window) {
			return new ContentType(settings, window);
		});
}

template <typename SettingsType>
template <typename ContentType, typename... ParamTypes>
void QtSourceGroupWizardPage<SettingsType>::addContentCreatorWithSettings(
	ParamTypes... params)
{
	addContentCreator(
		[=](std::shared_ptr<SettingsType> settings, QtProjectWizardWindow* window) {
			return new ContentType(settings, window, params...);
		});
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
QtProjectWizardContentGroup* QtSourceGroupWizardPage<SettingsType>::createContentGroup(
	std::shared_ptr<SettingsType> settings,
	QtProjectWizardWindow* window) const
{
	QtProjectWizardContentGroup* contentGroup = new QtProjectWizardContentGroup(window);

	bool firstContentAdded = false;
	for (const ContentCreator& contentCreator: m_contentCreators)
	{
		if (firstContentAdded)
		{
			contentGroup->addSpace();
		}
		contentGroup->addContent(contentCreator(settings, window));
		firstContentAdded = true;
	}

	return contentGroup;
}


#endif	  // QT_SOURCE_GROUP_WIZARD_PAGE_H
