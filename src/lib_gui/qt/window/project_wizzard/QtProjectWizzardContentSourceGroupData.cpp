#include "qt/window/project_wizzard/QtProjectWizzardContentSourceGroupData.h"

#include <QCheckBox>
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
	, m_status(nullptr)
{
}

void QtProjectWizzardContentSourceGroupData::populate(QGridLayout* layout, int& row)
{
	m_name = new QLineEdit();
	m_name->setObjectName("name");
	m_name->setAttribute(Qt::WA_MacShowFocusRect, 0);
	connect(m_name, &QLineEdit::textEdited, this, &QtProjectWizzardContentSourceGroupData::editedName);

	layout->addWidget(createFormLabel("Source Group Name"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_name, row, QtProjectWizzardWindow::BACK_COL);
	row++;

	m_status = new QCheckBox("active");
	connect(m_status, &QCheckBox::toggled, this, &QtProjectWizzardContentSourceGroupData::changedStatus);
	layout->addWidget(createFormLabel("Status"), row, QtProjectWizzardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_status, row, QtProjectWizzardWindow::BACK_COL);

	addHelpButton("Status", "<p>Only source files of active Source Groups will be processed during indexing. "
		"Inactive Source Groups will be ignored or cleared from the index.</p><p>Use this setting to temporarily "
		"remove files from your project (e.g. tests).</p>", layout, row);

	row++;
}

void QtProjectWizzardContentSourceGroupData::load()
{
	m_name->setText(QString::fromStdString(m_settings->getName()));

	m_status->setChecked(m_settings->getStatus() == SOURCE_GROUP_STATUS_ENABLED);
}

void QtProjectWizzardContentSourceGroupData::save()
{
	m_settings->setName(m_name->text().toStdString());

	m_settings->setStatus(m_status->isChecked() ? SOURCE_GROUP_STATUS_ENABLED : SOURCE_GROUP_STATUS_DISABLED);
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
	if (!m_status->isChecked())
	{
		name = "(" + name + ")";
	}

	emit nameUpdated(name);
}

void QtProjectWizzardContentSourceGroupData::changedStatus(bool checked)
{
	emit statusUpdated(m_status->isChecked() ? SOURCE_GROUP_STATUS_ENABLED : SOURCE_GROUP_STATUS_DISABLED);

	editedName(m_name->text());
}
