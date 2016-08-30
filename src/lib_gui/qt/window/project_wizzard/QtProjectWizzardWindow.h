#ifndef QT_PROJECT_WIZZARD_WINDOW_H
#define QT_PROJECT_WIZZARD_WINDOW_H

#include "qt/window/QtWindow.h"
#include "settings/ProjectSettings.h"

class QtProjectWizzardContent;

class QtProjectWizzardWindow
	: public QtWindow
{
	Q_OBJECT

public:
	QtProjectWizzardWindow(QWidget *parent);
	virtual QSize sizeHint() const override;

	QtProjectWizzardContent* content() const;
	void setContent(QtProjectWizzardContent* content);

	void setPreferredSize(QSize size);

	static const int FRONT_COL = 0;
	static const int HELP_COL = 1;
	static const int LINE_COL = 2;
	static const int BACK_COL = 3;

protected:
	// QtWindow implementation
	virtual void populateWindow(QWidget* widget) override;
	virtual void windowReady() override;

	virtual void handleNext() override;
	virtual void handlePrevious() override;

private:
	QtProjectWizzardContent* m_content;
	QSize m_preferredSize;
};

#endif // QT_PROJECT_WIZZARD_WINDOW_H
