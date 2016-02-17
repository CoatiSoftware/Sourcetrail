#include "qt/window/project_wizzard/QtProjectWizzardContent.h"

QtProjectWizzardContent::QtProjectWizzardContent(ProjectSettings* settings, QtProjectWizzardWindow* window)
	: QWidget(window)
	, m_settings(settings)
	, m_window(window)
{
}

void QtProjectWizzardContent::populateWindow(QWidget* widget)
{
}

void QtProjectWizzardContent::populateForm(QFormLayout* layout)
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

QLabel* QtProjectWizzardContent::createFormLabel(QString name) const
{
	QLabel* label = new QLabel(name);
	label->setAlignment(Qt::AlignRight);
	label->setObjectName("label");
	label->setWordWrap(true);
	return label;
}
