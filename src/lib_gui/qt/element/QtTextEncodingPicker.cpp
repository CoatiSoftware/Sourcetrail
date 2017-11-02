#include "qt/element/QtTextEncodingPicker.h"

#include <QHBoxLayout>
#include <QTextCodec>
#include <QFontComboBox>

QtTextEncodingPicker::QtTextEncodingPicker(QWidget *parent)
	: QWidget(parent)
{
	setObjectName("picker");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(1, 1, 1, 1);
	layout->setAlignment(Qt::AlignTop);

	m_box = new QComboBox();
	for (int mib : QTextCodec::availableMibs())
	{
		m_box->addItem(QTextCodec::codecForMib(mib)->name());
	}
	m_box->setEditable(false);

	layout->addWidget(m_box);

	setLayout(layout);
	setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
}

QString QtTextEncodingPicker::getText()
{
	return m_box->currentText();
}

void QtTextEncodingPicker::setText(QString text)
{
	m_box->setCurrentText(text);
}

