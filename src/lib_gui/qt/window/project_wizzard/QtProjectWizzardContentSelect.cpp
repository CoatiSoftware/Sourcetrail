#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"

#include <QButtonGroup>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>

#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "utility/ResourcePaths.h"

#include "utility/solution/SolutionParserManager.h"

QtProjectWizzardContentSelect::QtProjectWizzardContentSelect(
	std::shared_ptr<ProjectSettings> settings,
	QtProjectWizzardWindow* window,
	std::weak_ptr<SolutionParserManager> solutionParserManager
)
	: QtProjectWizzardContent(settings, window)
	, m_solutionParserManager(solutionParserManager)
{
}

void QtProjectWizzardContentSelect::populate(QGridLayout* layout, int& row)
{
	QPushButton* d = new QPushButton(languageTypeToString(LANGUAGE_CPP).c_str(), this);
	QPushButton* e = new QPushButton(languageTypeToString(LANGUAGE_C).c_str(), this);
	QPushButton* f = new QPushButton(languageTypeToString(LANGUAGE_JAVA).c_str(), this);

	d->setObjectName("menuButton");
	e->setObjectName("menuButton");
	f->setObjectName("menuButton");

	d->setCheckable(true);
	e->setCheckable(true);
	f->setCheckable(true);

	d->setChecked(true);

	m_languages = new QButtonGroup();
	m_languages->addButton(d);
	m_languages->addButton(e);
	m_languages->addButton(f);

	m_languages->setId(d, 0);
	m_languages->setId(e, 1);
	m_languages->setId(f, 2);

	connect(m_languages, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
		[this](int id)
		{
			bool isJava = stringToLanguageType(m_languages->checkedButton()->text().toStdString()) == LANGUAGE_JAVA;

			m_buttons->setExclusive(false);
			for (int i = 0; i < m_buttons->buttons().size(); i++)
			{
				m_buttons->button(i)->setChecked(false);

				if (i != 0)
				{
					m_buttons->button(i)->setVisible(!isJava);
				}
			}
			m_buttons->setExclusive(true);

			m_window->setNextEnabled(false);
			m_title->setText("Project Types - " + m_languages->checkedButton()->text());
			m_description->setText("");
		}
	);

	QVBoxLayout* vlayout = new QVBoxLayout();
	vlayout->setContentsMargins(0, 30, 0, 0);
	vlayout->setSpacing(10);

	vlayout->addWidget(d);
	vlayout->addWidget(e);
	vlayout->addWidget(f);

	vlayout->addStretch();

	layout->addLayout(vlayout, 0, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);


	QToolButton* a = createProjectButton(
		"Empty Project", (ResourcePaths::getGuiPath() + "icon/project_256_256.png").c_str());
	QToolButton* c = createProjectButton(
		"from Compilation\nDatabase", (ResourcePaths::getGuiPath() + "icon/project_cdb_256_256.png").c_str());

	m_solutionDescription.push_back("Create a new Coati project by defining what files will be indexed.");
	m_solutionDescription.push_back(
		"Create a project from an existing Compilation Database (compile_commands.json). They can be created from Make and "
		"CMake projects. Have a look at the <a href=\"https://coati.io/documentation/#CreateAProjectFromCompilationDatabase\">"
		"documentation</a> to find out more.");

	m_buttons = new QButtonGroup(this);
	m_buttons->addButton(a);
	m_buttons->addButton(c);

	m_buttons->setId(a, PROJECT_EMPTY);
	m_buttons->setId(c, PROJECT_CDB);

	QHBoxLayout* hlayout = new QHBoxLayout();

	hlayout->addWidget(a);
	hlayout->addWidget(c);

	unsigned int runningId = 2;
	std::shared_ptr<SolutionParserManager> manager = m_solutionParserManager.lock();
	if (manager != NULL)
	{
		for (unsigned int i = 0; i < manager->getParserCount(); i++)
		{
			std::string name = manager->getParserButtonText(i);

			QToolButton* button = createProjectButton(name.c_str(),
				(ResourcePaths::getGuiPath() + manager->getIconPath(i)).c_str());

			m_buttons->addButton(button);

			m_solutionDescription.push_back(manager->getParserDescription(i));

			hlayout->addWidget(button);

			m_buttons->setId(button, runningId);

			runningId++;
		}
	}

	// Add vs solution button
	{
		std::string name = "Visual Studio";

		QToolButton* button = createProjectButton(name.c_str(),
			(ResourcePaths::getGuiPath() + "icon/project_vs_256_256.png").c_str());

		m_buttons->addButton(button);

		m_solutionDescription.push_back("Create a new project from an existing Visual Studio Solution file. "
			"<b>Note: Requires a running Visual Studio instance with the "
			"<a href=\"https://coati.io/documentation/index.html#VisualStudio\">Visual Studio plugin</a> installed.");

		hlayout->addWidget(button);

		m_buttons->setId(button, runningId);

		runningId++;
	}

	connect(m_buttons, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
		[this](int id)
		{
			m_description->setText(m_solutionDescription[id].c_str());

			m_window->setNextEnabled(true);
		}
	);

	QFrame* container = new QFrame();
	container->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	container->setObjectName("projectContainer");
	container->setLayout(hlayout);

	layout->addWidget(container, 0, QtProjectWizzardWindow::BACK_COL);

	m_description = new QLabel(" \n \n");
	m_description->setWordWrap(true);
	m_description->setOpenExternalLinks(true);
	m_description->setObjectName("projectDescription");
	layout->addWidget(m_description, 1, QtProjectWizzardWindow::BACK_COL);

	m_title = new QLabel("Project Types - " + m_languages->checkedButton()->text());
	m_title->setObjectName("projectTitle");

	layout->addWidget(m_title, 0, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);

	layout->setRowStretch(0, 0);
	layout->setRowStretch(1, 1);
	layout->setColumnStretch(QtProjectWizzardWindow::FRONT_COL, 0);
	layout->setColumnStretch(QtProjectWizzardWindow::BACK_COL, 1);
	layout->setHorizontalSpacing(0);
}

void QtProjectWizzardContentSelect::save()
{
	ProjectType type;

	switch (m_buttons->checkedId())
	{
	case 0: type = PROJECT_EMPTY; break;
	case 1: type = PROJECT_CDB; break;
	case 2: type = PROJECT_MANAGED; break;
	default: type = PROJECT_MANAGED; break;
	}

	emit selected(stringToLanguageType(m_languages->checkedButton()->text().toStdString()), type);
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
