#ifndef QT_LOCATION_PICKER_H
#define QT_LOCATION_PICKER_H

#include <QPushButton>

#include "qt/element/QtLineEdit.h"

class QtLocationPicker
	: public QWidget
{
	Q_OBJECT

public:
	QtLocationPicker(QWidget *parent);

	QString getText();
	void setText(QString text);
	void clearText();

private slots:
	void handleButtonPress();

private:
	QPushButton* m_button;
	QtLineEdit* m_data;
};

#endif // QT_LOCATION_PICKER_H
