#include "QtProjectWizzardContentVS.h"

#include "MessageIDECreateCDB.h"

QtProjectWizzardContentVS::QtProjectWizzardContentVS(QtProjectWizzardWindow* window)
	: QtProjectWizzardContent(window)
{
}

void QtProjectWizzardContentVS::populate(QGridLayout* layout, int& row)
{
	QLabel* nameLabel = createFormLabel("Create Compilation Database");
	layout->addWidget(nameLabel, row, QtProjectWizzardWindow::FRONT_COL);

	addHelpButton(
		"Create Compilation Database", "To create a new Compilation Database from a Visual Studio Solution, this Solution has to be open in Visual Studio.\n\
Sourcetrail will call Visual Studio to open the 'Create Compilation Database' dialog.\
 Please follow the instructions in Visual Studio to complete the process.\n\
Note: Sourcetrail's Visual Studio plugin has to be installed. Visual Studio has to be running with an eligible Solution, containing C/C++ projects, loaded.", layout, row);

	QLabel* descriptionLabel = createFormLabel("Call Visual Studio to create a Compilation Database from the loaded Solution.");
	descriptionLabel->setObjectName("description");
	descriptionLabel->setAlignment(Qt::AlignmentFlag::AlignLeft);
	layout->addWidget(descriptionLabel, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	QPushButton* button = new QPushButton("Create Compilation Database");
	button->setObjectName("windowButton");
	layout->addWidget(button, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	QLabel* skipLabel = createFormLabel("*Skip this step if you already have a Compilation Database for your Solution.");
	skipLabel->setObjectName("description");
	skipLabel->setAlignment(Qt::AlignmentFlag::AlignLeft);
	layout->addWidget(skipLabel, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	addSeparator(layout, row++);

	connect(button, &QPushButton::clicked, this, &QtProjectWizzardContentVS::handleVSCDBClicked);
}

void QtProjectWizzardContentVS::handleVSCDBClicked()
{
	MessageIDECreateCDB().dispatch();
}
