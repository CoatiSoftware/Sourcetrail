#ifndef QT_LINE_EDIT_H
#define QT_LINE_EDIT_H

#include <QLineEdit>

class QtLineEdit: public QLineEdit
{
	Q_OBJECT

public:
	QtLineEdit(QWidget* parent = nullptr);

signals:
	void focus();

protected:
	void focusInEvent(QFocusEvent* event);
};

#endif	  // QT_LINE_EDIT_H
