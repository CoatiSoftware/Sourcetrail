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
	, m_headerPaths(nullptr)
	, m_frameworkPaths(nullptr)
{
	m_headerPaths = new QtProjectWizzardContentPathsHeaderSearchGlobal(nullptr, nullptr);

	if (QSysInfo::macVersion() != QSysInfo::MV_None)
	{
		m_frameworkPaths = new QtProjectWizzardContentPathsFrameworkSearchGlobal(nullptr, nullptr);
	}
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
	m_headerPaths->load();

	if (m_frameworkPaths)
	{
		m_frameworkPaths->load();
	}
}

void QtApplicationSettingsScreen::populateWindow(QWidget* widget)
{
	QFormLayout* layout = new QFormLayout();
	layout->setContentsMargins(10, 10, 10, 10);
	layout->setHorizontalSpacing(20);

	m_headerPaths->populateForm(layout);

	if (m_frameworkPaths)
	{
		m_frameworkPaths->populateForm(layout);
	}

	widget->setLayout(layout);
}

void QtApplicationSettingsScreen::handleCancelButtonPress()
{
	emit canceled();
}

void QtApplicationSettingsScreen::handleUpdateButtonPress()
{
	m_headerPaths->save();

	if (m_frameworkPaths)
	{
		m_frameworkPaths->save();
	}

	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();
	appSettings->save();

	MessageRefresh().dispatch();
	emit finished();
}
