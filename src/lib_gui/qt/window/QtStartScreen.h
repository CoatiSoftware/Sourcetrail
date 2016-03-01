#ifndef QT_START_SCREEN_H
#define QT_START_SCREEN_H

#include <QPushButton>

#include "qt/window/QtWindow.h"
#include "utility/file/FilePath.h"

class QtRecentProjectButton
	: public QPushButton
{
	Q_OBJECT

public:
	QtRecentProjectButton(const FilePath& projectFilePath, QWidget* parent);

public slots:
	void handleButtonClick();

private:
	FilePath m_projectFilePath;
};


class QtStartScreen
	: public QtWindow
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

private:
	QPushButton* m_openProjectButton;
	QPushButton* m_newProjectButton;
	std::vector<QPushButton*> m_recentProjects;
};

#endif // QT_START_SCREEN_H
