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
	void closed();

public:
	QtProjectWizzardWindow(QWidget *parent);

	QtProjectWizzardContent* content() const;
	void setContent(QtProjectWizzardContent* content);

	// QtSettingsWindow implementation
	virtual void setup() override;
	virtual void populateWindow(QWidget* widget) override;

	void enableNext();
	void disableNext();
	void hideNext();
	void disablePrevious();
	void hidePrevious();

	bool getShowAsPopup() const;
	void setShowAsPopup(bool showAsPopup);

private slots:
	void handleCancelButtonPress();
	void handleUpdateButtonPress();
	void handlePreviousButtonPress();

private:
	QtProjectWizzardContent* m_content;
	QPushButton* m_previousButton;

	bool m_showAsPopup;
};

#endif // QT_PROJECT_WIZZARD_WINDOW_H
