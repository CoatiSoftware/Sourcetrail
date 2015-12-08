#ifndef QT_START_SCREEN_H
#define QT_START_SCREEN_H

#include <QPushButton>

#include "qt/window/QtSettingsWindow.h"

class QtRecentProjectButton
	: public QPushButton
{
	Q_OBJECT

public:
	QtRecentProjectButton(const QString& text, QWidget* parent);

public slots:
	void handleButtonClick();

private:
	std::string m_projectFile;
};


class QtStartScreen
	: public QtSettingsWindow
{
	Q_OBJECT

public:
	QtStartScreen(QWidget* parent = 0);
	QSize sizeHint() const Q_DECL_OVERRIDE;
	virtual void setup() override;

signals:
	void openOpenProjectDialog();
	void openNewProjectDialog();

private slots:
	void handleNewProjectButton();
	void handleOpenProjectButton();
	void handleRecentButton();

private:
	QPushButton* m_openProjectButton;
	QPushButton* m_newProjectButton;
	std::vector<QPushButton*> m_recentProjects;
};

#endif // QT_START_SCREEN_H
