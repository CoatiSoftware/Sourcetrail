#include "qt/window/project_wizzard/QtProjectWizzardContentSourceGroupData.h"

#include <QLineEdit>
#include <QMessageBox>

#include "settings/SourceGroupSettings.h"

QtProjectWizzardContentSourceGroupData::QtProjectWizzardContentSourceGroupData(
	std::shared_ptr<SourceGroupSettings> settings,
	QtProjectWizzardWindow* window
)
	: QtProjectWizzardContent(window)
	, m_settings(settings)
	, m_name(nullptr)
{
}

void QtProjectWizzardContentSourceGroupData::populate(QGridLayout* layout, int& row)
{
	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		row++;
	}

	QLabel* nameLabel = createFormLabel("Source Group Name");
	m_name = new QLineEdit();
	m_name->setObjectName("name");
	m_name->setAttribute(Qt::WA_MacShowFocusRect, 0);
	connect(m_name, &QLineEdit::textEdited, this, &QtProjectWizzardContentSourceGroupData::editedName);

	layout->addWidget(nameLabel, row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_name, row, QtProjectWizzardWindow::BACK_COL);
	layout->setRowMinimumHeight(row, 30);
	row++;

	if (!isInForm())
	{
		layout->setRowMinimumHeight(row, 15);
		layout->setRowStretch(row, 1);
	}
}

void QtProjectWizzardContentSourceGroupData::load()
{
	m_name->setText(QString::fromStdString(m_settings->getName()));
}

void QtProjectWizzardContentSourceGroupData::save()
{
	m_settings->setName(m_name->text().toStdString());
}

bool QtProjectWizzardContentSourceGroupData::check()
{
	if (m_name->text().isEmpty())
	{
		QMessageBox msgBox;
		msgBox.setText("Please enter a source group name.");
		msgBox.exec();
		return false;
	}

	return true;
}

void QtProjectWizzardContentSourceGroupData::editedName(QString name)
{
	emit nameUpdated(name);
}
