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
	raise();
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

	QLabel* includePathsLabel = new QLabel("Include Paths");
	m_includePaths = new QtDirectoryListBox(this);
	m_includePaths->setMinimumWidth(minimumWidthForSecondCol);
	layout->addRow(includePathsLabel, m_includePaths);

	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		QLabel* frameworkPathsLabel = new QLabel("Framework Paths");
		m_frameworkPaths = new QtDirectoryListBox(this);
		m_frameworkPaths->setMinimumWidth(minimumWidthForSecondCol);
		layout->addRow(frameworkPathsLabel, m_frameworkPaths);
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
