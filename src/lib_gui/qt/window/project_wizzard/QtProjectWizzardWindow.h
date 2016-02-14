#ifndef QT_PROJECT_WIZZARD_WINDOW_H
#define QT_PROJECT_WIZZARD_WINDOW_H

#include "qt/window/QtSettingsWindow.h"
#include "settings/ProjectSettings.h"

class QPushButton;
class QtProjectWizzardContent;

class QtProjectWizzardWindow
	: public QtSettingsWindow
{
	Q_OBJECT

signals:
	void next();
	void previous();

public:
	QtProjectWizzardWindow(QWidget *parent);

	QtProjectWizzardContent* content() const;
	void setContent(QtProjectWizzardContent* content);

	// QtSettingsWindow implementation
	virtual void setup() override;
	virtual void populateWindow(QWidget* widget) override;

	void enableNext();
	void disableNext();
	void disablePrevious();
	void hidePrevious();

private:
	QtProjectWizzardContent* m_content;
	QPushButton* m_previousButton;

private slots:
	void handleCancelButtonPress();
	void handleUpdateButtonPress();
	void handlePreviousButtonPress();
};

#endif // QT_PROJECT_WIZZARD_WINDOW_H
