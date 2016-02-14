#include "qt/element/QtLocationPicker.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>

QtLocationPicker::QtLocationPicker(QWidget *parent)
	: QWidget(parent)
{
	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(1, 1, 1, 1);
	layout->setAlignment(Qt::AlignTop);

	setLayout(layout);

	m_data = new QtLineEdit(this);
	m_data->setAttribute(Qt::WA_MacShowFocusRect, 0);
	m_data->setObjectName("locationField");

	m_button = new QPushButton("...");
	m_button->setObjectName("moreButton");
	m_button->setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac

	layout->addWidget(m_data);
	layout->addWidget(m_button);

	connect(m_button, SIGNAL(clicked()), this, SLOT(handleButtonPress()));
}

QString QtLocationPicker::getText()
{
	return m_data->text();
}

void QtLocationPicker::setText(QString text)
{
	m_data->setText(text);
}

void QtLocationPicker::clearText()
{
	m_data->clear();
}

void QtLocationPicker::handleButtonPress()
{
	QString file = QFileDialog::getExistingDirectory(this, tr("Select Directory"), "");
	if (!file.isEmpty())
	{
		m_data->setText(file);
	}
}
