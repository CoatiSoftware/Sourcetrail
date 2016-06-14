#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"

#include <QCheckBox>
#include <QLabel>

QtProjectWizzardContentSimple::QtProjectWizzardContentSimple(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
	, m_checkBox(nullptr)
{
}

void QtProjectWizzardContentSimple::populateWindow(QGridLayout* layout)
{
	int row = 0;
	populateWindow(layout, row);
}

void QtProjectWizzardContentSimple::populateWindow(QGridLayout* layout, int& row)
{
	QLabel* title = new QLabel("Lazy Include Search");
	title->setWordWrap(true);
	title->setObjectName("section");
	layout->addWidget(title, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row, 0);

	QLabel* text = new QLabel("Search for included files in all subdirectories of the project paths. "
		"Warning: This slows down indexing speed.");
	text->setWordWrap(true);
	layout->addWidget(text, row + 1, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop);
	layout->setRowStretch(row + 1, 1);

	m_checkBox = new QCheckBox("Search included files within the project paths");
	layout->addWidget(m_checkBox, row, QtProjectWizzardWindow::BACK_COL);

	row += 2;
}

void QtProjectWizzardContentSimple::populateForm(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel("Lazy Include Search");
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL);

	m_checkBox = new QCheckBox("Search included files within the project paths");
	layout->addWidget(m_checkBox, row, QtProjectWizzardWindow::BACK_COL);

	addHelpButton("Check this box to search for included files in all subdirectories of the project paths. "
		"This avoids setting them manually if you are not familiar with the project you want to index.\n"
		"Warning: This option slows down indexing speed.", layout, row);

	row++;
}

void QtProjectWizzardContentSimple::load()
{
	m_checkBox->setChecked(m_settings->getUseSourcePathsForHeaderSearch());
}

void QtProjectWizzardContentSimple::save()
{
	m_settings->setUseSourcePathsForHeaderSearch(m_checkBox->isChecked());
}
