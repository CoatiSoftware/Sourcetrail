#ifndef QT_FONT_PICKER_H
#define QT_FONT_PICKER_H

#include <QWidget>

class QPushButton;
class QtLineEdit;
class QComboBox;
class QLabel;

class QtFontPicker
	: public QWidget
{
	Q_OBJECT

public:
	QtFontPicker(QWidget *parent);

	virtual void paintEvent(QPaintEvent*) override;

	QString getText();
	void setText(QString text);

private:
	QLabel* m_sampleLabel;
	QComboBox* m_box;
};

#endif // QT_FONT_PICKER_H
