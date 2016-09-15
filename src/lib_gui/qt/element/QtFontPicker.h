#ifndef QT_FONT_PICKER_H
#define QT_FONT_PICKER_H

#include <QWidget>

class QFontComboBox;

class QtFontPicker
	: public QWidget
{
	Q_OBJECT

public:
	QtFontPicker(QWidget *parent);

	QString getText();
	void setText(QString text);

private:
	QFontComboBox* m_box;
};

#endif // QT_FONT_PICKER_H
