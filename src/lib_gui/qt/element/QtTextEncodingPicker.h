#ifndef QT_TEXT_ENCODING_PICKER_H
#define QT_TEXT_ENCODING_PICKER_H

#include <QWidget>

class QComboBox;

class QtTextEncodingPicker
	: public QWidget
{
	Q_OBJECT

public:
	QtTextEncodingPicker(QWidget *parent);

	QString getText();
	void setText(QString text);

private:
	QComboBox* m_box;
};

#endif // QT_TEXT_ENCODING_PICKER_H
