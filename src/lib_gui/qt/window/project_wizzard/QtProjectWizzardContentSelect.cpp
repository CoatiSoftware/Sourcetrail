#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"

#include <QButtonGroup>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>

#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "utility/ResourcePaths.h"

#include "utility/solution/SolutionParserManager.h"

QtProjectWizzardContentSelect::QtProjectWizzardContentSelect(ProjectSettings* settings, QtProjectWizzardWindow* window, std::weak_ptr<SolutionParserManager> solutionParserManager)
	: QtProjectWizzardContent(settings, window)
	, m_solutionParserManager(solutionParserManager)
{
}

void QtProjectWizzardContentSelect::populateWindow(QGridLayout* layout)
{
	QPushButton* d = new QPushButton("C++");
	QPushButton* e = new QPushButton("C");

	d->setObjectName("menuButton");
	e->setObjectName("menuButton");

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

			m_window->setNextEnabled(false);
			m_title->setText("Project Types - " + m_languages->checkedButton()->text());
			m_description->setText("");
		}
	);

	QVBoxLayout* vlayout = new QVBoxLayout();
	vlayout->setContentsMargins(0, 30, 0, 0);

	vlayout->addWidget(d);
	vlayout->addWidget(e);

	vlayout->addStretch();

	layout->addLayout(vlayout, 0, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);


	QToolButton* a = createProjectButton(
		"empty project", (ResourcePaths::getGuiPath() + "icon/project_256_256.png").c_str());
	QToolButton* b = createProjectButton(
		"from Visual\nStudio Solution", (ResourcePaths::getGuiPath() + "icon/project_vs_256_256.png").c_str());
	QToolButton* c = createProjectButton(
		"from Compilation\nDatabase", (ResourcePaths::getGuiPath() + "icon/project_cdb_256_256.png").c_str());


	m_solutionDescription.push_back("Create a new Coati project by defining what files will be analyzed and header search paths.");
	m_solutionDescription.push_back("Create a new project from an existing Visual Studio Solution file.");
	m_solutionDescription.push_back("Create a project from an existing Compilation Database. Compilation Databases can be created from "
					"cmake projects. Have a look at the "
					"<a href=\"https://staging.coati.io/documentation/#CreateAProjectFromCompilationDatabase\">"
					"documentation</a> to find out more.");

	m_buttons = new QButtonGroup(this);
	m_buttons->addButton(a);
	m_buttons->addButton(b);
	m_buttons->addButton(c);

	m_buttons->setId(a, PROJECT_EMPTY);
	m_buttons->setId(b, PROJECT_VS);
	m_buttons->setId(c, PROJECT_CDB);

	QHBoxLayout* hlayout = new QHBoxLayout();

	hlayout->addWidget(a);
	hlayout->addWidget(b);
	hlayout->addWidget(c);

	unsigned int runningId = 3;
	std::shared_ptr<SolutionParserManager> manager = m_solutionParserManager.lock();
	if (manager != NULL)
	{
		for (unsigned int i = 0; i < manager->getParserCount(); i++)
		{
			std::string name = manager->getParserName(i);

			QToolButton* button = createProjectButton(name.c_str(),
				(ResourcePaths::getGuiPath() + "icon/project_vs_256_256.png").c_str());

			m_buttons->addButton(button);

			m_solutionDescription.push_back(manager->getParserDescription(i));

			hlayout->addWidget(button);

			m_buttons->setId(button, runningId);

			runningId++;
		}
	}

	connect(m_buttons, static_cast<void(QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
		[this](int id)
		{
			/*switch (id)
			{
			case 0: m_description->setText(
					"Create a new Coati project by defining what files will be analyzed and header search paths."
				); break;
			case 1: m_description->setText(
					"Create a new project from an existing Visual Studio Solution file."
				); break;
			case 2: m_description->setText(
					"Create a project from an existing Compilation Database. Compilation Databases can be created from "
					"cmake projects. Have a look at the "
					"<a href=\"https://staging.coati.io/documentation/#CreateAProjectFromCompilationDatabase\">"
					"documentation</a> to find out more."
				); break;
			}*/

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
	m_settings->setLanguage(m_languages->checkedButton()->text().toStdString());

	ProjectType type;

	switch (m_buttons->checkedId())
	{
	case 0: type = PROJECT_EMPTY; break;
	case 1: type = PROJECT_VS; break;
	case 2: type = PROJECT_CDB; break;
	default: type = PROJECT_VS; break; // use vs for "standard" solutions TODO: change name of enum to reflect this is the default type
	}

	emit selected(type);

	// emit selected(m_buttons->checkedId());
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

QSize QtProjectWizzardContentSelect::preferredWindowSize() const
{
	return QSize(570, 380);
}
