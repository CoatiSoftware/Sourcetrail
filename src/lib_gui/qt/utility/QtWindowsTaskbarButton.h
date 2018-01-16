#ifndef QT_WINDOWS_TASKBAR_BUTTON_H
#define QT_WINDOWS_TASKBAR_BUTTON_H

class QWinTaskbarProgress;
class QtMainWindow;

class QtWindowsTaskbarButton
{
public:
	QtWindowsTaskbarButton();
	void setWindow(QtMainWindow* mainWindow);

	void setProgress(float progress);
	void hideProgress();

private:

#ifdef _WIN32
	QWinTaskbarProgress* m_taskbarProgress;
#endif
};

#endif // QT_WINDOWS_TASKBAR_BUTTON_H
