#ifndef QT_START_SCREEN_H
#define QT_START_SCREEN_H

#include <QPushButton>

#include "FilePath.h"
#include "QtWindow.h"

class QtRecentProjectButton: public QPushButton
{
	Q_OBJECT

public:
	QtRecentProjectButton(QWidget* parent);
	bool projectExists() const;
	void setProjectPath(const FilePath& projectFilePath);
public slots:
	void handleButtonClick();
signals:
	void updateButtons();

private:
	bool m_projectExists;
	FilePath m_projectFilePath;
};


class QtStartScreen: public QtWindow
{
	Q_OBJECT

public:
	QtStartScreen(QWidget* parent = 0);
	QSize sizeHint() const override;

	void setupStartScreen();

signals:
	void openOpenProjectDialog();
	void openNewProjectDialog();

private slots:
	void handleNewProjectButton();
	void handleOpenProjectButton();
	void handleRecentButton();
	void updateButtons();

private:
	std::vector<QtRecentProjectButton*> m_recentProjectsButtons;
	const QIcon m_cppIcon;
	const QIcon m_cIcon;
	const QIcon m_pythonIcon;
	const QIcon m_javaIcon;
	const QIcon m_projectIcon;
	const QIcon m_githubIcon;
};

#endif	  // QT_START_SCREEN_H
