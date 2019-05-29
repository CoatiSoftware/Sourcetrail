#ifndef QT_TEXT_EDIT_H
#define QT_TEXT_EDIT_H

#include <QTextBrowser>

class QtTextEdit
	: public QTextBrowser
{
	Q_OBJECT

public:
	QtTextEdit(QWidget* parent = nullptr);

	void setViewportMargins(int left, int top, int right, int bottom);

signals:
	void focus();

protected:
	void focusInEvent(QFocusEvent* event);
};

#endif // QT_TEXT_EDIT_H
