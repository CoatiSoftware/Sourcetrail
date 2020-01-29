#include "QtTextEdit.h"

QtTextEdit::QtTextEdit(QWidget* parent): QTextBrowser(parent)
{
	document()->setDefaultStyleSheet(QStringLiteral("a { color: #007AC2; }"));
}

void QtTextEdit::setViewportMargins(int left, int top, int right, int bottom)
{
	QAbstractScrollArea::setViewportMargins(left, top, right, bottom);
}

void QtTextEdit::focusInEvent(QFocusEvent* event)
{
	emit focus();
	QTextBrowser::focusInEvent(event);
}
