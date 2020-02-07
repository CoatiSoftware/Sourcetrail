#ifndef QT_LOCATION_PICKER_H
#define QT_LOCATION_PICKER_H

#include <QPushButton>

#include "FilePath.h"
#include "QtLineEdit.h"

class QtLocationPicker: public QWidget
{
	Q_OBJECT

public:
	QtLocationPicker(QWidget* parent);

	virtual void paintEvent(QPaintEvent*) override;

	void setPlaceholderText(const QString& text);
	QString getText();
	void setText(const QString& text);
	void clearText();

	bool pickDirectory() const;
	void setPickDirectory(bool pickDirectory);

	void setFileFilter(const QString& fileFilter);

	void setRelativeRootDirectory(const FilePath& dir);

signals:
	void locationPicked();
	void textChanged(const QString& text);

protected:
	void changeEvent(QEvent* event) override;

private:
	void onHandleButtonPressed();
	void onDataTextChanged(const QString& text);

	QPushButton* m_button;
	QtLineEdit* m_data;

	bool m_pickDirectory;
	QString m_fileFilter;
	FilePath m_relativeRootDirectory;
};

#endif	  // QT_LOCATION_PICKER_H
