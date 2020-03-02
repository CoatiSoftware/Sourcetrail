#include "QtProjectWizardContentSourceGroupData.h"

#include <QCheckBox>
#include <QLineEdit>
#include <QMessageBox>

#include "SourceGroupSettings.h"

QtProjectWizardContentSourceGroupData::QtProjectWizardContentSourceGroupData(
	std::shared_ptr<SourceGroupSettings> settings, QtProjectWizardWindow* window)
	: QtProjectWizardContent(window), m_settings(settings), m_name(nullptr), m_status(nullptr)
{
	setIsRequired(true);
}

void QtProjectWizardContentSourceGroupData::populate(QGridLayout* layout, int& row)
{
	m_name = new QLineEdit();
	m_name->setObjectName(QStringLiteral("name"));
	m_name->setAttribute(Qt::WA_MacShowFocusRect, 0);
	connect(m_name, &QLineEdit::textEdited, this, &QtProjectWizardContentSourceGroupData::editedName);

	layout->addWidget(
		createFormLabel(QStringLiteral("Source Group Name")),
		row,
		QtProjectWizardWindow::FRONT_COL,
		Qt::AlignRight);
	layout->addWidget(m_name, row, QtProjectWizardWindow::BACK_COL);
	row++;

	m_status = new QCheckBox(QStringLiteral("active"));
	connect(
		m_status, &QCheckBox::toggled, this, &QtProjectWizardContentSourceGroupData::changedStatus);
	layout->addWidget(
		createFormSubLabel(QStringLiteral("Status")), row, QtProjectWizardWindow::FRONT_COL, Qt::AlignRight);
	layout->addWidget(m_status, row, QtProjectWizardWindow::BACK_COL);

	addHelpButton(
		QStringLiteral("Status"),
		"<p>Only source files of active Source Groups will be processed during indexing. "
		"Inactive Source Groups will be ignored or cleared from the index.</p><p>Use this setting "
		"to temporarily "
		"remove files from your project (e.g. tests).</p>",
		layout,
		row);

	row++;
}

void QtProjectWizardContentSourceGroupData::load()
{
	m_name->setText(QString::fromStdString(m_settings->getName()));

	m_status->setChecked(m_settings->getStatus() == SOURCE_GROUP_STATUS_ENABLED);
}

void QtProjectWizardContentSourceGroupData::save()
{
	m_settings->setName(m_name->text().toStdString());

	m_settings->setStatus(
		m_status->isChecked() ? SOURCE_GROUP_STATUS_ENABLED : SOURCE_GROUP_STATUS_DISABLED);
}

bool QtProjectWizardContentSourceGroupData::check()
{
	if (m_name->text().isEmpty())
	{
		QMessageBox msgBox(m_window);
		msgBox.setText(QStringLiteral("Please enter a source group name."));
		msgBox.exec();
		return false;
	}

	return true;
}

void QtProjectWizardContentSourceGroupData::editedName(QString name)
{
	if (!m_status->isChecked())
	{
		name = "(" + name + ")";
	}

	emit nameUpdated(name);
}

void QtProjectWizardContentSourceGroupData::changedStatus(bool checked)
{
	emit statusUpdated(
		m_status->isChecked() ? SOURCE_GROUP_STATUS_ENABLED : SOURCE_GROUP_STATUS_DISABLED);

	editedName(m_name->text());
}
