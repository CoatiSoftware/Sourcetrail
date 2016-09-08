#include "qt/element/QtFontPicker.h"

#include <QFileDialog>
#include <QFontDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QStyleOption>
#include <QComboBox>
#include <QFontDatabase>

#include "qt/element/QtLineEdit.h"


QtFontPicker::QtFontPicker(QWidget *parent)
	: QWidget(parent)
{
	setObjectName("picker");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(1, 1, 1, 1);
	layout->setAlignment(Qt::AlignTop);

	m_box = new QComboBox();
	QFontDatabase database;
	m_box->setFont(QFont("Roboto"));
	for( QString fam : database.families() )
	{
		m_box->addItem(fam);
	}

	QLabel* sampleLabel = new QLabel("Font preview:");
	m_sampleLabel = new QLabel("AaBbCcXxYyZz");
	layout->addWidget(sampleLabel);
	layout->addWidget(m_sampleLabel);
	QBoxLayout* layout1 = new QVBoxLayout();
	layout1->addWidget(m_box);
	layout1->addLayout(layout);

	connect(m_box, static_cast<void(QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged),
		[=](const QString &family)
		{
			if (!family.isEmpty())
			{
				m_sampleLabel->setFont(QFont(family));
			}
		});

	setLayout(layout1);
	setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
}

void QtFontPicker::paintEvent(QPaintEvent*)
{
	QStyleOption opt;
	opt.init(this);

	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

QString QtFontPicker::getText()
{
	return m_box->currentText();
}

void QtFontPicker::setText(QString text)
{
	m_box->setCurrentText(text);
}

