#ifndef QT_HELP_BUTTON_H
#define QT_HELP_BUTTON_H

#include "qt/element/QtIconButton.h"

class QtHelpButton
	: public QtIconButton
{
	Q_OBJECT

public:
	QtHelpButton(const QString& helpText, QWidget* parent = nullptr);

private slots:
	void handleHelpPress();

private:
	QString m_helpText;
};

#endif // QT_HELP_BUTTON_H
