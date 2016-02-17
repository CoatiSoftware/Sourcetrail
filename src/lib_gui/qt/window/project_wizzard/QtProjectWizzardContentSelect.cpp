#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"

#include <QButtonGroup>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>

#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"

QtProjectWizzardContentSelect::QtProjectWizzardContentSelect(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentSelect::populateWindow(QWidget* widget)
{
	QVBoxLayout* vlayout = new QVBoxLayout();

	QLabel* title = new QLabel("Project Type");
	title->setObjectName("label");
	vlayout->addWidget(title);

	QRadioButton* a = new QRadioButton("empty project");
	QRadioButton* b = new QRadioButton("from Visual Studio Solution");
	QRadioButton* c = new QRadioButton("from Compilation Database");

	m_buttons = new QButtonGroup(this);
	m_buttons->addButton(a);
	m_buttons->addButton(b);
	m_buttons->addButton(c);

	m_buttons->setId(a, PROJECT_EMPTY);
	m_buttons->setId(b, PROJECT_VS);
	m_buttons->setId(c, PROJECT_CDB);

	connect(m_buttons, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
		[this](int id)
		{
			m_window->enableNext();
		}
	);

	vlayout->addWidget(a);
	vlayout->addWidget(b);
	vlayout->addWidget(c);

	vlayout->addStretch();


	QVBoxLayout* vlayout2 = new QVBoxLayout();


	QPushButton* d = new QPushButton("C++");
	QPushButton* e = new QPushButton("C");

	d->setCheckable(true);
	e->setCheckable(true);

	d->setChecked(true);

	m_languages = new QButtonGroup();
	m_languages->addButton(d);
	m_languages->addButton(e);

	m_languages->setId(d, 0);
	m_languages->setId(e, 1);

	connect(m_languages, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
		[this](int id)
		{
			m_buttons->setExclusive(false);
			for (int i = 0; i < m_buttons->buttons().size(); i++)
			{
				m_buttons->button(i)->setChecked(false);
			}
			m_buttons->setExclusive(true);

			m_window->disableNext();
		}
	);

	vlayout2->addWidget(d);
	vlayout2->addWidget(e);
	vlayout2->addStretch();

	QHBoxLayout* hlayout = new QHBoxLayout();

	hlayout->addLayout(vlayout2);
	hlayout->addLayout(vlayout);

	widget->setLayout(hlayout);
}

void QtProjectWizzardContentSelect::save()
{
	m_settings->setLanguage(m_languages->checkedButton()->text().toStdString());

	ProjectType type;

	switch (m_buttons->checkedId())
	{
	case 0: type = PROJECT_EMPTY; break;
	case 1: type = PROJECT_VS; break;
	case 2: type = PROJECT_CDB; break;
	}

	emit selected(type);
}

bool QtProjectWizzardContentSelect::check()
{
	if (m_buttons->checkedId() == -1)
	{
		QMessageBox msgBox;
		msgBox.setText("Please choose a method of creating a new project.");
		msgBox.exec();
		return false;
	}

	return true;
}
