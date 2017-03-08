#include "qt/window/project_wizzard/QtProjectWizzardContentSelect.h"

#include <QButtonGroup>
#include <QMessageBox>
#include <QLabel>
#include <QPushButton>

#include "qt/window/project_wizzard/QtProjectWizzardWindow.h"
#include "utility/ResourcePaths.h"
#include "utility/utilityString.h"

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
	struct ProjectInfo
	{
		ProjectInfo(ProjectType type) :type(type) {}
		const ProjectType type;
	};

	// define which kind of projects are available for each language
	std::map<LanguageType, std::vector<ProjectInfo>> projectInfos;
	projectInfos[LANGUAGE_C].push_back(ProjectInfo(PROJECT_C_EMPTY));
	projectInfos[LANGUAGE_C].push_back(ProjectInfo(PROJECT_CXX_CDB));
	projectInfos[LANGUAGE_C].push_back(ProjectInfo(PROJECT_CXX_VS));
	projectInfos[LANGUAGE_CPP].push_back(ProjectInfo(PROJECT_CPP_EMPTY));
	projectInfos[LANGUAGE_CPP].push_back(ProjectInfo(PROJECT_CXX_CDB));
	projectInfos[LANGUAGE_CPP].push_back(ProjectInfo(PROJECT_CXX_VS));
	projectInfos[LANGUAGE_JAVA].push_back(ProjectInfo(PROJECT_JAVA_EMPTY));
	projectInfos[LANGUAGE_JAVA].push_back(ProjectInfo(PROJECT_JAVA_MAVEN));

	// define which icons should be used for which kind of project
	m_projectTypeIconName[PROJECT_C_EMPTY] = "empty_icon";
	m_projectTypeIconName[PROJECT_CPP_EMPTY] = "empty_icon";
	m_projectTypeIconName[PROJECT_CXX_CDB] = "cdb_icon";
	m_projectTypeIconName[PROJECT_CXX_VS] = "vs_icon";
	m_projectTypeIconName[PROJECT_JAVA_EMPTY] = "empty_icon";
	m_projectTypeIconName[PROJECT_JAVA_MAVEN] = "empty_icon";

	// define descriptions for each kind of project
	m_projectTypeDescriptions[PROJECT_C_EMPTY] = "Create a new Coati project by defining which C files will be indexed.";
	m_projectTypeDescriptions[PROJECT_CPP_EMPTY] = "Create a new Coati project by defining which C++ files will be indexed.";
	m_projectTypeDescriptions[PROJECT_CXX_CDB] = "Create a project from an existing Compilation Database (compile_commands.json). They can be created from Make and "
		"CMake projects. Have a look at the <a href=\"https://coati.io/documentation/#CreateAProjectFromCompilationDatabase\">"
		"documentation</a> to find out more.";
	m_projectTypeDescriptions[PROJECT_CXX_VS] = "Create a new project from an existing Visual Studio Solution file. "
		"<b>Note: Requires a running Visual Studio instance with the "
		"<a href=\"https://coati.io/documentation/index.html#VisualStudio\">Visual Studio plugin</a> installed.";
	m_projectTypeDescriptions[PROJECT_JAVA_EMPTY] = "Create a new Coati project by defining which Java files will be indexed.";
	m_projectTypeDescriptions[PROJECT_JAVA_MAVEN] = "Create a new project from an existing Maven project.";

	QVBoxLayout* vlayout = new QVBoxLayout();
	vlayout->setContentsMargins(0, 30, 0, 0);
	vlayout->setSpacing(10);

	m_languages = new QButtonGroup();
	for (auto it: projectInfos)
	{
		QPushButton* b = new QPushButton(languageTypeToString(it.first).c_str(), this);
		b->setObjectName("menuButton");
		b->setCheckable(true);
		b->setProperty("language_type", it.first);
		m_languages->addButton(b);
		vlayout->addWidget(b);
	}

	vlayout->addStretch();
	layout->addLayout(vlayout, 0, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);

	connect(m_languages, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
		[this](QAbstractButton* button)
		{
			LanguageType selectedLanguage = LANGUAGE_UNKNOWN;
			bool ok = false;
			int languageTypeInt = button->property("language_type").toInt(&ok);
			if (ok)
			{
				selectedLanguage = LanguageType(languageTypeInt);
			}

			for (auto it: m_buttons)
			{
				it.second->setExclusive(false);
				for (QAbstractButton* button: it.second->buttons())
				{
					button->setChecked(false);
					button->setVisible(it.first == selectedLanguage);
				}
				it.second->setExclusive(true);
			}

			m_window->setNextEnabled(false);
			m_title->setText("Project Types - " + m_languages->checkedButton()->text());
			m_description->setText("");
		}
	);

	QHBoxLayout* hlayout = new QHBoxLayout();

	for (auto languageIt: projectInfos)
	{
		QButtonGroup* projectButtons = new QButtonGroup(this);

		for (auto projectIt: languageIt.second)
		{
			QToolButton* b = createProjectButton(
				utility::insertLineBreaksAtBlankSpaces(projectTypeToString(projectIt.type), 15).c_str(),
				(ResourcePaths::getGuiPath() + "icon/" + m_projectTypeIconName[projectIt.type] + ".png").c_str()
			);
			b->setProperty("project_type", int(projectIt.type));
			projectButtons->addButton(b);
			hlayout->addWidget(b);
		}

		m_buttons[languageIt.first] = projectButtons;
	}

	for (auto it: m_buttons)
	{
		connect(it.second, static_cast<void(QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
			[this](QAbstractButton* button)
			{
				ProjectType selectedProject = PROJECT_UNKNOWN;
				bool ok = false;
				int projectTypeInt = button->property("project_type").toInt(&ok);
				if (ok)
				{
					selectedProject = ProjectType(projectTypeInt);
				}

				m_description->setText(m_projectTypeDescriptions[selectedProject].c_str());

				m_window->setNextEnabled(true);
			}
		);
	}

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

	m_title = new QLabel("Project Types");
	m_title->setObjectName("projectTitle");

	layout->addWidget(m_title, 0, QtProjectWizzardWindow::BACK_COL, Qt::AlignLeft | Qt::AlignTop);

	layout->setRowStretch(0, 0);
	layout->setRowStretch(1, 1);
	layout->setColumnStretch(QtProjectWizzardWindow::FRONT_COL, 0);
	layout->setColumnStretch(QtProjectWizzardWindow::BACK_COL, 1);
	layout->setHorizontalSpacing(0);

	m_languages->buttons().first()->click();
}

void QtProjectWizzardContentSelect::save()
{
	ProjectType type;

	for (auto it: m_buttons)
	{
		if (QAbstractButton* b = it.second->checkedButton())
		{
			bool ok = false;
			int projectType = b->property("project_type").toInt(&ok);
			if (ok)
			{
				type = ProjectType(projectType);
				break;
			}
		}
	}
	emit selected(type);
}

bool QtProjectWizzardContentSelect::check()
{
	bool projectChosen = false;

	for (auto it: m_buttons)
	{
		if (it.second->checkedId() != -1)
		{
			projectChosen = true;
			break;
		}
	}

	if (!projectChosen)
	{
		QMessageBox msgBox;
		msgBox.setText("Please choose a method of creating a new project.");
		msgBox.exec();
		return false;
	}

	return true;
}
