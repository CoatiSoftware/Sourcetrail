#ifndef QT_TEXT_EDIT_DIALOG_H
#define QT_TEXT_EDIT_DIALOG_H

#include "qt/window/QtWindow.h"

class QPlainTextEdit;

class QtTextEditDialog
	: public QtWindow
{
	Q_OBJECT

public:
	QtTextEditDialog(const QString& title, const QString& description, QWidget* parent = 0);

	QSize sizeHint() const override;

	void setText(const std::string& text);
	std::string getText();

	void setReadOnly(bool readOnly);

protected:
	void populateWindow(QWidget* widget) override;
	void windowReady() override;

	QString m_title;
	QString m_description;

private:
	QPlainTextEdit* m_text;
};

#endif // QT_TEXT_EDIT_DIALOG_H
