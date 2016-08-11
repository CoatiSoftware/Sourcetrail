#include "qt/window/project_wizzard/QtProjectWizzardContentPreferences.h"

#include "settings/ApplicationSettings.h"
#include "utility/messaging/type/MessageSwitchColorScheme.h"
#include "utility/file/FileSystem.h"
#include "utility/ResourcePaths.h"

QtProjectWizzardContentPreferences::QtProjectWizzardContentPreferences(
	std::shared_ptr<ProjectSettings> settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
	, m_oldColorSchemeIndex(-1)
{
	std::vector<std::string> colorSchemePaths =
		FileSystem::getFileNamesFromDirectory(ResourcePaths::getColorSchemesPath(), std::vector<std::string>(1, ".xml"));
	for (const std::string& colorScheme : colorSchemePaths)
	{
		m_colorSchemePaths.push_back(FilePath(colorScheme));
	}
}

QtProjectWizzardContentPreferences::~QtProjectWizzardContentPreferences()
{
	if (m_oldColorSchemeIndex != -1)
	{
		colorSchemeChanged(m_oldColorSchemeIndex);
	}
}

void QtProjectWizzardContentPreferences::populateWindow(QGridLayout* layout)
{
	int row = 0;
	populateForm(layout, row);
}

void QtProjectWizzardContentPreferences::populateForm(QGridLayout* layout, int& row)
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	// ui
	layout->addWidget(createFormTitle("USER INTERFACE"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignLeft);
	row++;

	// font face
	m_fontFace = new QLineEdit();
	m_fontFace->setObjectName("name");
	m_fontFace->setAttribute(Qt::WA_MacShowFocusRect, 0);

	layout->addWidget(createFormLabel("Font face"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_fontFace, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	// font size
	m_fontSize = new QComboBox();
	for (int i = appSettings->getFontSizeMin(); i <= appSettings->getFontSizeMax(); i++)
	{
		m_fontSize->insertItem(i, QString::number(i));
	}

	layout->addWidget(createFormLabel("Font size"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_fontSize, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	// tab width
	m_tabWidth = new QComboBox();
	for (int i = 1; i < 17; i++)
	{
		m_tabWidth->insertItem(i, QString::number(i));
	}

	layout->addWidget(createFormLabel("Tab width"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_tabWidth, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	// color scheme
	m_colorSchemes = new QComboBox();
	for (size_t i = 0; i < m_colorSchemePaths.size(); i++)
	{
		m_colorSchemes->insertItem(i, m_colorSchemePaths[i].withoutExtension().fileName().c_str());
	}

	connect(m_colorSchemes, SIGNAL(activated(int)), this, SLOT(colorSchemeChanged(int)));

	layout->addWidget(createFormLabel("Color scheme"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_colorSchemes, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	layout->setRowMinimumHeight(row++, 20);


	// indexing
	layout->addWidget(createFormTitle("INDEXING"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignLeft);
	row++;

	// indexer threads
	m_threads = new QComboBox();
	for (int i = 1; i <= 24; i++)
	{
		m_threads->insertItem(i, QString::number(i));
	}

	layout->addWidget(createFormLabel("Indexer threads"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_threads, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);

	addHelpButton(
		"Number of parallel threads used to index your projects."
		, layout, row
	);

	row++;

	// ignore non-fatal errors in non-indexed files
	m_fatalErrors = new QCheckBox("Display non-fatal errors in unindexed files", this);

	layout->addWidget(createFormLabel("Non-Fatal Errors"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_fatalErrors, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);

	addHelpButton(
		"When checked non-fatal errors within included unindexed files are also shown."
		, layout, row
	);

	row++;
}

void QtProjectWizzardContentPreferences::load()
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	m_fontFace->setText(QString::fromStdString(appSettings->getFontName()));

	m_fontSize->setCurrentIndex(appSettings->getFontSize() - appSettings->getFontSizeMin());
	m_tabWidth->setCurrentIndex(appSettings->getCodeTabWidth() - 1);

	FilePath colorSchemePath = appSettings->getColorSchemePath();
	for (size_t i = 0; i < m_colorSchemePaths.size(); i++)
	{
		if (colorSchemePath == m_colorSchemePaths[i])
		{
			m_colorSchemes->setCurrentIndex(i);
			m_oldColorSchemeIndex = i;
			break;
		}
	}

	m_threads->setCurrentIndex(appSettings->getIndexerThreadCount() - 1);
	m_fatalErrors->setChecked(appSettings->getShowExternalNonFatalErrors());
}

void QtProjectWizzardContentPreferences::save()
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	appSettings->setFontName(m_fontFace->text().toStdString());

	appSettings->setFontSize(m_fontSize->currentIndex() + appSettings->getFontSizeMin());
	appSettings->setCodeTabWidth(m_tabWidth->currentIndex() + 1);

	appSettings->setColorSchemePath(m_colorSchemePaths[m_colorSchemes->currentIndex()]);
	m_oldColorSchemeIndex = -1;

	appSettings->setIndexerThreadCount(m_threads->currentIndex() + 1);
	appSettings->setShowExternalNonFatalErrors(m_fatalErrors->isChecked());
}

bool QtProjectWizzardContentPreferences::check()
{
	return true;
}

void QtProjectWizzardContentPreferences::colorSchemeChanged(int index)
{
	MessageSwitchColorScheme(m_colorSchemePaths[index]).dispatch();
}
