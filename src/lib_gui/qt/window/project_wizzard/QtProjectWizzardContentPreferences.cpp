#include "qt/window/project_wizzard/QtProjectWizzardContentPreferences.h"

#include "settings/ApplicationSettings.h"

QtProjectWizzardContentPreferences::QtProjectWizzardContentPreferences(
	ProjectSettings* settings, QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(settings, window)
{
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
	QLabel* fontLabel = createFormLabel("Font face");
	m_fontFace = new QLineEdit();
	m_fontFace->setObjectName("name");
	m_fontFace->setAttribute(Qt::WA_MacShowFocusRect, 0);

	layout->addWidget(fontLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_fontFace, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	// font size
	QLabel* sizeLabel = createFormLabel("Font size");

	m_fontSize = new QComboBox();
	for (int i = appSettings->getFontSizeMin(); i <= appSettings->getFontSizeMax(); i++)
	{
		m_fontSize->insertItem(i, QString::number(i));
	}

	layout->addWidget(sizeLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_fontSize, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	// tab width
	QLabel* tabLabel = createFormLabel("Tab width");

	m_tabWidth = new QComboBox();
	for (int i = 1; i < 17; i++)
	{
		m_tabWidth->insertItem(i, QString::number(i));
	}

	layout->addWidget(tabLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_tabWidth, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);
	row++;

	layout->setRowMinimumHeight(row++, 20);


	// indexing
	layout->addWidget(createFormTitle("INDEXING"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignLeft);
	row++;

	// indexer threads
	QLabel* threadsLabel = createFormLabel("Indexer threads");

	m_threads = new QComboBox();
	for (int i = 1; i <= 24; i++)
	{
		m_threads->insertItem(i, QString::number(i));
	}

	layout->addWidget(threadsLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_threads, row, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft);

	addHelpButton(
		"Number of parallel threads used to index your projects."
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

	m_threads->setCurrentIndex(appSettings->getIndexerThreadCount() - 1);
}

void QtProjectWizzardContentPreferences::save()
{
	ApplicationSettings* appSettings = ApplicationSettings::getInstance().get();

	appSettings->setFontName(m_fontFace->text().toStdString());

	appSettings->setFontSize(m_fontSize->currentIndex() + appSettings->getFontSizeMin());
	appSettings->setCodeTabWidth(m_tabWidth->currentIndex() + 1);

	appSettings->setIndexerThreadCount(m_threads->currentIndex() + 1);
}

bool QtProjectWizzardContentPreferences::check()
{
	return true;
}
