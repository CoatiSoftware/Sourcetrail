#include "QtWindowsTaskbarButton.h"

#ifdef _WIN32
#	include <QWinTaskbarButton>
#	include <QWinTaskbarProgress>
#endif

#include "QtMainWindow.h"

QtWindowsTaskbarButton::QtWindowsTaskbarButton()
#ifdef _WIN32
	: m_taskbarProgress(nullptr)
#endif
{
}

void QtWindowsTaskbarButton::setWindow(QtMainWindow* mainWindow)
{
#ifdef _WIN32
	QWinTaskbarButton* taskbarButton = new QWinTaskbarButton(mainWindow);
	taskbarButton->setWindow(mainWindow->windowHandle());
	m_taskbarProgress = taskbarButton->progress();
#endif
}

void QtWindowsTaskbarButton::setProgress(float progress)
{
#ifdef _WIN32
	if (m_taskbarProgress != nullptr)
	{
		m_taskbarProgress->show();
		m_taskbarProgress->setValue(
			static_cast<int>(std::max(0, std::min<int>(100, static_cast<int>(100 * progress)))));
	}
#endif
}

void QtWindowsTaskbarButton::hideProgress()
{
#ifdef _WIN32
	if (m_taskbarProgress != nullptr)
	{
		m_taskbarProgress->hide();
	}
#endif
}
