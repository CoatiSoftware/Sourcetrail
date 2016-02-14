#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"

#include <QButtonGroup>
#include <QFormLayout>
#include <QMessageBox>
#include <QLabel>
#include <QRadioButton>

#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"

QtProjectWizzardContentSelect::QtProjectWizzardContentSelect(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentSelect::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout(widget);

	QLabel* title = new QLabel("project type");
	title->setObjectName("label");
	layout->addWidget(title);

	QRadioButton* a = new QRadioButton("empty");
	QRadioButton* b = new QRadioButton("from Visual Studio Solution");
	QRadioButton* c = new QRadioButton("from Compilation Database");

	m_buttons = new QButtonGroup(this);
	m_buttons->addButton(a);
	m_buttons->addButton(b);
	m_buttons->addButton(c);

	m_buttons->setId(a, PROJECT_EMPTY);
	m_buttons->setId(b, PROJECT_CDB);
	m_buttons->setId(c, PROJECT_VS);

	connect(m_buttons, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
		[this](int id)
		{
			m_window->enableNext();
		}
	);

	layout->addWidget(a);
	layout->addWidget(b);
	layout->addWidget(c);

	layout->addStretch();

	widget->setLayout(layout);
}

bool QtProjectWizzardContentSelect::check()
{
	if (m_buttons->checkedId() == -1)
	{
		QMessageBox msgBox;
		msgBox.setText("Please choose how you want to create your project.");
		msgBox.exec();
		return false;
	}

	ProjectType type;

	switch (m_buttons->checkedId())
	{
	case 0: type = PROJECT_EMPTY; break;
	case 1: type = PROJECT_CDB; break;
	case 2: type = PROJECT_VS; break;
	}

	emit selected(type);

	return true;
}
