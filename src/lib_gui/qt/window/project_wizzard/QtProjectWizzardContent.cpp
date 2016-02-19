#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

#include <QMessageBox>

QtHelpButton::QtHelpButton(const QString& helpText, QWidget* parent)
	: QPushButton("", parent)
	, m_helpText(helpText)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac
	setObjectName("helpButton");

	connect(this, SIGNAL(clicked()), this, SLOT(handleHelpPress()));
}

void QtHelpButton::handleHelpPress()
{
	QMessageBox msgBox;
	msgBox.setText("Help");
	msgBox.setInformativeText(m_helpText);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	msgBox.exec();
}


QtProjectWizzardContent::QtProjectWizzardContent(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QWidget(window)
	, m_settings(settings)
	, m_window(window)
{
}

void QtProjectWizzardContent::populateWindow(QWidget* widget)
{
}

void QtProjectWizzardContent::populateWindow(QGridLayout* layout)
{
}

void QtProjectWizzardContent::populateForm(QGridLayout* layout, int& row)
{
}

void QtProjectWizzardContent::windowReady()
{
}

void QtProjectWizzardContent::load()
{
}

void QtProjectWizzardContent::save()
{
}

bool QtProjectWizzardContent::check()
{
	return true;
}

bool QtProjectWizzardContent::isScrollAble() const
{
	return false;
}

QSize QtProjectWizzardContent::preferredWindowSize() const
{
	return QSize(600, 620);
}

QLabel* QtProjectWizzardContent::createFormLabel(QString name) const
{
	QLabel* label = new QLabel(name);
	label->setAlignment(Qt::AlignRight);
	label->setObjectName("label");
	label->setWordWrap(true);
	return label;
}

QToolButton* QtProjectWizzardContent::createProjectButton(QString name, QString iconPath) const
{
	QToolButton* button = new QToolButton();
	button->setObjectName("projectButton");
	button->setText(name);
	button->setIcon(QPixmap(iconPath));
	button->setIconSize(QSize(64, 64));
	button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
	button->setCheckable(true);
	return button;
}

QtHelpButton* QtProjectWizzardContent::addHelpButton(QString helpString, QGridLayout* layout, int row) const
{
	QtHelpButton* button = new QtHelpButton(helpString);
	layout->addWidget(button, row, QtProjectWizzardWindow::HELP_COL, Qt::AlignTop);
	return button;
}
