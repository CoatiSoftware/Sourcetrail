#include "qt/element/QtLineEdit.h"

QtLineEdit::QtLineEdit(QWidget* parent)
	: QLineEdit(parent)
{
}

void QtLineEdit::focusInEvent(QFocusEvent* event)
{
	emit focus();
	QLineEdit::focusInEvent(event);
}
