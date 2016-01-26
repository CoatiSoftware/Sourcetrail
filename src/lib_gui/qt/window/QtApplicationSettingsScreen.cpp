#include "qt/window/QtApplicationSettingsScreen.h"

#include <QComboBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QSysInfo>

#include "utility/messaging/type/MessageLoadProject.h"
#include "utility/messaging/type/MessageRefresh.h"

#include "settings/ApplicationSettings.h"

QtApplicationSettingsScreen::QtApplicationSettingsScreen(QWidget *parent)
	: QtSettingsWindow(parent)
	, m_frameworkPaths(nullptr)
{
}

QSize QtApplicationSettingsScreen::sizeHint() const
{
	return QSize(600,600);
}

void QtApplicationSettingsScreen::setup()
{
	setupForm();

	updateTitle("PREFERENCES");
	updateDoneButton("Save");
}

void QtApplicationSettingsScreen::load()
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	m_includePaths->setList(appSettings->getHeaderSearchPaths());

	if (m_frameworkPaths)
	{
		m_frameworkPaths->setList(appSettings->getFrameworkSearchPaths());
	}
}

void QtApplicationSettingsScreen::populateForm(QFormLayout* layout)
{
	int minimumWidthForSecondCol = 360;
	QPushButton* helpButton;

	QWidget* includePathsWidget = createLabelWithHelpButton("Header\nSearch Paths", &helpButton);
	connect(helpButton, SIGNAL(clicked()), this, SLOT(handleIncludePathHelpPress()));
	m_includePaths = new QtDirectoryListBox(this);
	m_includePaths->setMinimumWidth(minimumWidthForSecondCol);
	layout->addRow(includePathsWidget, m_includePaths);

	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		QWidget* frameworkPathsWidget = createLabelWithHelpButton("Framework\nSearch Paths", &helpButton);
		connect(helpButton, SIGNAL(clicked()), this, SLOT(handleFrameworkPathHelpPress()));
		m_frameworkPaths = new QtDirectoryListBox(this);
		m_frameworkPaths->setMinimumWidth(minimumWidthForSecondCol);
		layout->addRow(frameworkPathsWidget, m_frameworkPaths);
	}
}

void QtApplicationSettingsScreen::handleCancelButtonPress()
{
	emit canceled();
}

void QtApplicationSettingsScreen::handleUpdateButtonPress()
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	appSettings->setHeaderSearchPaths(m_includePaths->getList());

	if (m_frameworkPaths)
	{
		appSettings->setFrameworkSearchPaths(m_frameworkPaths->getList());
	}

	appSettings->save();

	MessageRefresh().dispatch();
	emit finished();
}

void QtApplicationSettingsScreen::handleIncludePathHelpPress()
{
	showHelpMessage(
		"Header Search Paths define where additional headers, that your project depends on, are found. Usually they are "
		"header files of frameworks or libraries that your project uses. These files won't be analysed, but Coati needs "
		"them for correct analysis.\n\n"
		"Header Search Paths defined here will be used for all projects."
	);
}

void QtApplicationSettingsScreen::handleFrameworkPathHelpPress()
{
	showHelpMessage(
		"Framework Search Paths define where MacOS framework containers, that your project depends on, are found.\n\n"
		"Framework Search Paths defined here will be used for all projects."
	);
}
