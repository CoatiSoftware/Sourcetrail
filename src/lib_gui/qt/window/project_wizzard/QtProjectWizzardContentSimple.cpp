#include "qt/window/project_wizzard/QtProjectWizzardContentSimple.h"

#include <QButtonGroup>
#include <QVBoxLayout>
#include <QLabel>
#include <QRadioButton>

QtProjectWizzardContentSimple::QtProjectWizzardContentSimple(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(settings, window)
{
}

void QtProjectWizzardContentSimple::populateWindow(QWidget* widget)
{
	QVBoxLayout* layout = new QVBoxLayout(widget);

	QLabel* title = new QLabel("simple setup");
	title->setObjectName("label");
	layout->addWidget(title);

	QLabel* text = new QLabel(
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do "
		"eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut "
		"enim ad minim veniam, quis nostrud exercitation ullamco laboris "
		"nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
		"reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
		"pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
		"culpa qui officia deserunt mollit anim id est laborum."
	);
	text->setWordWrap(true);
	layout->addWidget(text);

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

	layout->addWidget(a);
	layout->addWidget(b);

	layout->addStretch();

	widget->setLayout(layout);
}

void QtProjectWizzardContentSimple::load()
{
}

void QtProjectWizzardContentSimple::save()
{
}

bool QtProjectWizzardContentSimple::check()
{
	return true;
}
