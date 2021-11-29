#include "QtProjectWizardContentVS.h"

#include "MessageIDECreateCDB.h"
#include "utilityApp.h"

QtProjectWizardContentVS::QtProjectWizardContentVS(QtProjectWizardWindow* window)
	: QtProjectWizardContent(window)
{
}

void QtProjectWizardContentVS::populate(QGridLayout* layout, int& row)
{
	layout->setRowMinimumHeight(row++, 10);
	QLabel* nameLabel = createFormLabel(QStringLiteral("Create Compilation Database"));
	layout->addWidget(nameLabel, row, QtProjectWizardWindow::FRONT_COL);

	addHelpButton(
		QStringLiteral("Create Compilation Database"),
		QStringLiteral("To create a new Compilation Database from a Visual Studio Solution, a "
					   "Solution has to be open in Visual "
					   "Studio.\n Sourcetrail will call Visual Studio to open the 'Create "
					   "Compilation Database' dialog. Please follow "
					   "the instructions in Visual Studio to complete the process.\n Note: "
					   "Sourcetrail's Visual Studio plugin has to "
					   "be installed. Visual Studio has to be running with an eligible Solution, "
					   "containing C/C++ projects, loaded."),
		layout,
		row);

	QLabel* descriptionLabel = createFormSubLabel(QString::fromStdString(
		"Call Visual Studio to create a Compilation Database from the loaded Solution (requires "
		"installed "
		"<a href=\"" +
		utility::getDocumentationLink() +
		"#visual-studio\">Sourcetrail  Visual Studio  Extension</a>)."));
	descriptionLabel->setObjectName(QStringLiteral("description"));
	descriptionLabel->setOpenExternalLinks(true);
	descriptionLabel->setAlignment(Qt::AlignmentFlag::AlignLeft);
	layout->addWidget(descriptionLabel, row, QtProjectWizardWindow::BACK_COL);
	row++;

	QPushButton* button = new QPushButton(QStringLiteral("Create Compilation Database"));
	button->setObjectName(QStringLiteral("windowButton"));
	layout->addWidget(button, row, QtProjectWizardWindow::BACK_COL);
	row++;

	QLabel* skipLabel = createFormLabel(QStringLiteral(
		"*Skip this step if you already have a Compilation Database for your Solution."));
	skipLabel->setObjectName(QStringLiteral("description"));
	skipLabel->setAlignment(Qt::AlignmentFlag::AlignLeft);
	layout->addWidget(skipLabel, row, QtProjectWizardWindow::BACK_COL);
	row++;

	layout->setRowMinimumHeight(row, 10);
	layout->setRowStretch(row, 1);

	connect(button, &QPushButton::clicked, this, &QtProjectWizardContentVS::handleVSCDBClicked);
}

void QtProjectWizardContentVS::handleVSCDBClicked()
{
	MessageIDECreateCDB().dispatch();
}
