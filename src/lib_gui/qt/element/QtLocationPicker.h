#ifndef QT_LOCATION_PICKER_H
#define QT_LOCATION_PICKER_H

#include <QPushButton>

#include "qt/element/QtLineEdit.h"
#include "utility/file/FilePath.h"

class QtLocationPicker
	: public QWidget
{
	Q_OBJECT

public:
	QtLocationPicker(QWidget *parent);

	virtual void paintEvent(QPaintEvent*) override;

	void setPlaceholderText(QString text);
	QString getText();
	void setText(QString text);
	void clearText();

	void setPickDirectory(bool pickDirectory);
	void setFileFilter(const QString& fileFilter);

	void setRelativeRootDirectory(const FilePath& dir);

signals:
	void locationPicked();

protected:
	void changeEvent(QEvent *event) override;

private slots:
	void handleButtonPress();

private:
	QPushButton* m_button;
	QtLineEdit* m_data;

	bool m_pickDirectory;
	QString m_fileFilter;
	FilePath m_relativeRootDirectory;
};

#endif // QT_LOCATION_PICKER_H
