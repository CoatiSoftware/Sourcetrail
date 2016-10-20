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


class QtStartScreen
	: public QtWindow
{
	Q_OBJECT

public:
	QtStartScreen(QWidget* parent = 0);
	QSize sizeHint() const override;

	void setupStartScreen(bool unlocked);

signals:
	void openOpenProjectDialog();
	void openNewProjectDialog();
	void openEnterLicenseDialog();

private slots:
	void handleNewProjectButton();
	void handleOpenProjectButton();
	void handleUnlockButton();
	void handleRecentButton();
	void updateButtons();

private:
	std::vector<QtRecentProjectButton*> m_recentProjectsButtons;
	QIcon* m_cppIcon;
	QIcon* m_cIcon;
	QIcon* m_javaIcon;
	QIcon* m_projectIcon;
};

#endif // QT_START_SCREEN_H
