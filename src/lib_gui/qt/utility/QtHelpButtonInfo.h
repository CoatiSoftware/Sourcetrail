#ifndef QT_HELP_BUTTON_INFO_H
#define QT_HELP_BUTTON_INFO_H

#include <QString>

class QtHelpButtonInfo
{
public:
	QtHelpButtonInfo(const QString& helpTitle, const QString& helpText);

	void displayMessage();

private:
	QString m_title;
	QString m_text;
};

QtHelpButtonInfo createErrorHelpButtonInfo();

#endif	  // QT_HELP_BUTTON_INFO_H
