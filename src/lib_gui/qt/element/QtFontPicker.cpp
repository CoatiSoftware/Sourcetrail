#include "qt/element/QtFontPicker.h"

#include <QHBoxLayout>
#include <QPainter>
#include <QFontComboBox>

#include "qt/element/QtLineEdit.h"


QtFontPicker::QtFontPicker(QWidget *parent)
	: QWidget(parent)
{
	setObjectName("picker");

	QBoxLayout* layout = new QHBoxLayout();
	layout->setSpacing(0);
	layout->setContentsMargins(1, 1, 1, 1);
	layout->setAlignment(Qt::AlignTop);

	m_box = new QFontComboBox();
	m_box->setFontFilters( QFontComboBox::MonospacedFonts );
	m_box->setEditable(false);

	layout->addWidget(m_box);

	setLayout(layout);
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

