#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"

#include <QCheckBox>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QRadioButton>

QtProjectWizzardContentSimple::QtProjectWizzardContentSimple(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
	, m_buttons(nullptr)
	, m_checkBox(nullptr)
	, m_isForm(false)
{
}

void QtProjectWizzardContentSimple::populateWindow(QGridLayout* layout)
{
	layout->setRowMinimumHeight(0, 10);

	QLabel* title = new QLabel("Simple Setup");
	title->setObjectName("section");
	layout->addWidget(title, 1, QtProjectWizzardWindow::FRONT_COL, Qt::AlignTop | Qt::AlignRight);

	QLabel* text = new QLabel(
		"In simple setup you just provide the directory of your project and Coati will find all source files and "
		"resolve header search paths within. Please note that simple setup slows down the analysis.\n\n"
		"In the advanced setup you define analyzed source files and the corresponding header search paths separately."
	);
	text->setWordWrap(true);
	layout->addWidget(text, 1, QtProjectWizzardWindow::BACK_COL);

	QRadioButton* a = new QRadioButton("simple setup");
	QRadioButton* b = new QRadioButton("advanced setup");

	m_buttons = new QButtonGroup(this);
	m_buttons->addButton(a);
	m_buttons->addButton(b);

	m_buttons->setId(a, 0);
	m_buttons->setId(b, 1);

	connect(m_buttons, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
		[this](int id)
		{
			m_window->enableNext();
		}
	);

	QVBoxLayout* vlayout = new QVBoxLayout();

	vlayout->addWidget(a);
	vlayout->addWidget(b);

	vlayout->addStretch();

	layout->addLayout(vlayout, 2, QtProjectWizzardWindow::BACK_COL);

	layout->setColumnStretch(QtProjectWizzardWindow::FRONT_COL, 1);
	layout->setColumnStretch(QtProjectWizzardWindow::BACK_COL, 3);
}

void QtProjectWizzardContentSimple::populateForm(QGridLayout* layout, int& row)
{
	QLabel* label = createFormLabel("Search headers in project paths");
	layout->addWidget(label, row, QtProjectWizzardWindow::FRONT_COL);

	m_checkBox = new QCheckBox();
	layout->addWidget(m_checkBox, row, QtProjectWizzardWindow::BACK_COL);

	m_isForm = true;
	row++;
}

void QtProjectWizzardContentSimple::windowReady()
{
	if (!m_isForm)
	{
		m_window->disableNext();
	}
}

void QtProjectWizzardContentSimple::load()
{
	if (m_isForm && m_checkBox)
	{
		m_checkBox->setChecked(m_settings->getUseSourcePathsForHeaderSearch());
	}
	else if (m_buttons && m_settings->isUseSourcePathsForHeaderSearchDefined())
	{
		m_buttons->button(m_settings->getUseSourcePathsForHeaderSearch() ? 0 : 1)->setChecked(true);
		m_window->enableNext();
	}
}

void QtProjectWizzardContentSimple::save()
{
	bool simpleSetup;

	if (m_isForm)
	{
		simpleSetup = m_checkBox->isChecked();
	}
	else
	{
		switch (m_buttons->checkedId())
		{
		case 0: simpleSetup = true; break;
		case 1: simpleSetup = false; break;
		default: return;
		}
	}

	m_settings->setUseSourcePathsForHeaderSearch(simpleSetup);
}

bool QtProjectWizzardContentSimple::check()
{
	if (!m_isForm && m_buttons->checkedId() == -1)
	{
		QMessageBox msgBox;
		msgBox.setText("Please choose if you want simple or advanced setup.");
		msgBox.exec();
		return false;
	}

	return true;
}

QSize QtProjectWizzardContentSimple::preferredWindowSize() const
{
	return QSize(580, 350);
}
