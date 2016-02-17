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

	void setPickDirectory(bool pickDirectory);
	void setFileFilter(const QString& fileFilter);

private slots:
	void handleButtonPress();

private:
	QPushButton* m_button;
	QtLineEdit* m_data;

	bool m_pickDirectory;
	QString m_fileFilter;
};

#endif // QT_LOCATION_PICKER_H
