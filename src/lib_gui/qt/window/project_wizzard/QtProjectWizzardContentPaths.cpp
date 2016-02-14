#include "qt/window/project_wizzard/QtProjectWizzardContentPaths.h"

#include <QFormLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

#include "qt/element/QtDirectoryListBox.h"
#include "settings/ApplicationSettings.h"

QtHelpButton::QtHelpButton(const QString& helpText, QWidget* parent)
	: QPushButton("?", parent)
	, m_helpText(helpText)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	setObjectName("help");

	connect(this, SIGNAL(clicked()), this, SLOT(handleHelpPress()));
}

void QtHelpButton::handleHelpPress()
{
	QMessageBox msgBox;
	msgBox.setText("Help");
	msgBox.setInformativeText(m_helpText);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.exec();
}


QtProjectWizzardContentPaths::QtProjectWizzardContentPaths(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
	, m_subPaths(nullptr)
{
}

void QtProjectWizzardContentPaths::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout(widget);

	populateLayout(layout);

	if (m_subPaths)
	{
		layout->addSpacing(30);

		m_subPaths->populateLayout(layout);
	}

	layout->addStretch();
}

void QtProjectWizzardContentPaths::populateLayout(QVBoxLayout* layout)
{
	QLabel* label = new QLabel(m_titleString);
	label->setObjectName("label");
	layout->addWidget(label);

	QLabel* text = new QLabel(m_descriptionString);
	text->setWordWrap(true);
	layout->addWidget(text);

	m_list = new QtDirectoryListBox(this);
	layout->addWidget(m_list);
}

void QtProjectWizzardContentPaths::populateForm(QFormLayout* layout)
{
	QWidget* widget = new QWidget();

	QVBoxLayout* vlayout = new QVBoxLayout();
	vlayout->setContentsMargins(0, 5, 0, 5);
	vlayout->setSpacing(5);

	QLabel* label = new QLabel(m_titleString);
	label->setAlignment(Qt::AlignRight);
	label->setObjectName("label");
	label->setWordWrap(true);
	vlayout->addWidget(label);

	QtHelpButton* button = new QtHelpButton(m_helpString);

	vlayout->addWidget(button, 0, Qt::AlignRight);
	vlayout->addStretch();

	widget->setLayout(vlayout);

	int minimumWidthForSecondCol = 360;
	m_list = new QtDirectoryListBox(this);
	m_list->setMinimumWidth(minimumWidthForSecondCol);
	layout->addRow(widget, m_list);

	if (m_subPaths)
	{
		m_subPaths->populateForm(layout);
	}
}

void QtProjectWizzardContentPaths::load()
{
	loadPaths();

	if (m_subPaths)
	{
		return m_subPaths->loadPaths();
	}
}

void QtProjectWizzardContentPaths::save()
{
	savePaths();

	if (m_subPaths)
	{
		return m_subPaths->savePaths();
	}
}

bool QtProjectWizzardContentPaths::check()
{
	if (m_subPaths)
	{
		return checkPaths() && m_subPaths->checkPaths();
	}

	return checkPaths();
}

void QtProjectWizzardContentPaths::loadPaths()
{
}

void QtProjectWizzardContentPaths::savePaths()
{
}

bool QtProjectWizzardContentPaths::checkPaths()
{
	return true;
}

void QtProjectWizzardContentPaths::setInfo(const QString& title, const QString& description, const QString& help)
{
	m_titleString = title;
	m_descriptionString = description;
	m_helpString = help;
}


QtProjectWizzardContentPathsSource::QtProjectWizzardContentPathsSource(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Analyzed Paths",
		"Analyzed Paths define the source files and directories that will be analysed by Coati. Usually these are the "
		"source and header files of your project or a subset of them.",
		"Analyzed Paths define the source files and directories that will be analysed by Coati. Usually these are the "
		"source and header files of your project or a subset of them."
	);
}

void QtProjectWizzardContentPathsSource::loadPaths()
{
	m_list->setList(m_settings->getSourcePaths());
}

void QtProjectWizzardContentPathsSource::savePaths()
{
	m_settings->setSourcePaths(m_list->getList());
}

bool QtProjectWizzardContentPathsSource::checkPaths()
{
	if (m_list->getList().size() == 0)
	{
		QMessageBox msgBox;
		msgBox.setText("Please set at least one source path for Coati to analyse.");
		msgBox.exec();
		return false;
	}

	return true;
}


QtProjectWizzardContentPathsHeaderSearch::QtProjectWizzardContentPathsHeaderSearch(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Header Search Paths",
		"Header Search Paths define where additional headers, that your project depends on, are found. Usually they are "
		"header files of frameworks or libraries that your project uses. These files won't be analysed, but Coati needs "
		"them for correct analysis.",
		"Header Search Paths define where additional headers, that your project depends on, are found. Usually they are "
		"header files of frameworks or libraries that your project uses. These files won't be analysed, but Coati needs "
		"them for correct analysis.\n\n"
		"Header Search Paths defined here will be used for all projects."
	);

	m_subPaths = new QtProjectWizzardContentPathsHeaderSearchGlobal(settings, window);
}

void QtProjectWizzardContentPathsHeaderSearch::loadPaths()
{
	m_list->setList(m_settings->getHeaderSearchPaths());
}

void QtProjectWizzardContentPathsHeaderSearch::savePaths()
{
	m_settings->setHeaderSearchPaths(m_list->getList());
}

QtProjectWizzardContentPathsHeaderSearchGlobal::QtProjectWizzardContentPathsHeaderSearchGlobal(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Global Header Search Paths",
		"Header Search Paths for all projects.",
		"Header Search Paths define where additional headers, that your project depends on, are found. Usually they are "
		"header files of frameworks or libraries that your project uses. These files won't be analysed, but Coati needs "
		"them for correct analysis.\n\n"
		"Header Search Paths defined here will be used for all projects."
	);
}

void QtProjectWizzardContentPathsHeaderSearchGlobal::loadPaths()
{
	m_list->setList(ApplicationSettings::getInstance()->getHeaderSearchPaths());
}

void QtProjectWizzardContentPathsHeaderSearchGlobal::savePaths()
{
	ApplicationSettings::getInstance()->setHeaderSearchPaths(m_list->getList());
}


QtProjectWizzardContentPathsFrameworkSearch::QtProjectWizzardContentPathsFrameworkSearch(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Framework Search Paths",
		"Framework Search Paths define where MacOS framework containers, that your project depends on, are found.",
		"Framework Search Paths define where MacOS framework containers, that your project depends on, are found.\n\n"
		"Framework Search Paths defined here will be used for all projects."
	);

	m_subPaths = new QtProjectWizzardContentPathsFrameworkSearchGlobal(settings, window);
}

void QtProjectWizzardContentPathsFrameworkSearch::loadPaths()
{
	m_list->setList(m_settings->getFrameworkSearchPaths());
}

void QtProjectWizzardContentPathsFrameworkSearch::savePaths()
{
	m_settings->setFrameworkSearchPaths(m_list->getList());
}

QtProjectWizzardContentPathsFrameworkSearchGlobal::QtProjectWizzardContentPathsFrameworkSearchGlobal(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContentPaths(settings, window)
{
	setInfo(
		"Global Framework Search Paths",
		"Framework Search Paths for all projects",
		"Framework Search Paths define where MacOS framework containers, that your project depends on, are found.\n\n"
		"Framework Search Paths defined here will be used for all projects."
	);
}

void QtProjectWizzardContentPathsFrameworkSearchGlobal::loadPaths()
{
	m_list->setList(ApplicationSettings::getInstance()->getFrameworkSearchPaths());
}

void QtProjectWizzardContentPathsFrameworkSearchGlobal::savePaths()
{
	ApplicationSettings::getInstance()->setFrameworkSearchPaths(m_list->getList());
}
