#include "qt/element/QtCodeFileTitleButton.h"

#include <QApplication>
#include <QClipboard>
#include <QDesktopServices>
#include <QUrl>

#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageActivateFile.h"
#include "utility/messaging/type/MessageProjectEdit.h"

#include "Application.h"
#include "qt/utility/QtContextMenu.h"
#include "qt/utility/utilityQt.h"
#include "settings/ColorScheme.h"
#include "utility/ResourcePaths.h"

QtCodeFileTitleButton::QtCodeFileTitleButton(QWidget* parent)
	: QPushButton(parent)
{
	setObjectName("title_label");
	minimumSizeHint(); // force font loading
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac

	setFixedHeight(std::max(fontMetrics().height() * 1.2, 28.0));
	setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);

	connect(this, SIGNAL(clicked()), this, SLOT(clickedTitle()));

	m_copyFullPathAction = new QAction(tr("Copy Full Path"), this);
	m_copyFullPathAction->setStatusTip(tr("Copies the path of this file to the clipboard"));
	m_copyFullPathAction->setToolTip(tr("Copies the path of this file to the clipboard"));
	connect(m_copyFullPathAction, SIGNAL(triggered()), this, SLOT(copyFullPath()));

	m_openContainingFolderAction = new QAction(tr("Open Containing Folder"), this);
	m_openContainingFolderAction->setStatusTip(tr("Opens the folder that contains this file"));
	m_openContainingFolderAction->setToolTip(tr("Opens the folder that contains this file"));
	connect(m_openContainingFolderAction, SIGNAL(triggered()), this, SLOT(openContainingFolder()));
}

QtCodeFileTitleButton::~QtCodeFileTitleButton()
{
}

void QtCodeFileTitleButton::setFilePath(const FilePath& filePath)
{
	m_filePath = filePath;

	setText(filePath.fileName().c_str());
	setToolTip(filePath.str().c_str());

	std::string text = ResourcePaths::getGuiPath() + "code_view/images/file.png";

	setIcon(utility::colorizePixmap(
		QPixmap(text.c_str()),
		ColorScheme::getInstance()->getColor("code/file/title/icon").c_str()
	));
}

void QtCodeFileTitleButton::setModificationTime(const TimePoint modificationTime)
{
	if (modificationTime.isValid())
	{
		m_modificationTime = modificationTime;
		checkModification();
	}
}

void QtCodeFileTitleButton::setProject(const std::string& name)
{
	setText(name.c_str());

	if (Application::getInstance()->isInTrial())
	{
		setEnabled(false);
	}
	else
	{
		std::string text = ResourcePaths::getGuiPath() + "code_view/images/edit.png";
		setToolTip("edit project");

		setIcon(utility::colorizePixmap(
			QPixmap(text.c_str()),
			ColorScheme::getInstance()->getColor("code/file/title/icon").c_str()
		));
	}
}

void QtCodeFileTitleButton::checkModification()
{
	if (Application::getInstance()->isInTrial() || m_filePath.empty())
	{
		return;
	}

	// cannot use m_filePath.exists() here since it is only checked when FilePath is constructed.
	if ((!FileSystem::exists(m_filePath.str())) ||
		(FileSystem::getLastWriteTime(m_filePath) > m_modificationTime))
	{
		setText(QString(m_filePath.fileName().c_str()) + "*");
		setToolTip(QString::fromStdString("out of date: " + m_filePath.str()));
	}
	else
	{
		setText(m_filePath.fileName().c_str());
		setToolTip(QString::fromStdString(m_filePath.str()));
	}
}

void QtCodeFileTitleButton::contextMenuEvent(QContextMenuEvent* event)
{
	if (!m_filePath.empty())
	{
		std::vector<QAction*> actions;
		actions.push_back(m_copyFullPathAction);
		actions.push_back(m_openContainingFolderAction);
		QtContextMenu::getInstance()->showExtended(event, this, actions);
	}
	else if (text().size())
	{
		QPushButton::contextMenuEvent(event);
	}
}

void QtCodeFileTitleButton::clickedTitle()
{
	if (!m_filePath.empty())
	{
		MessageActivateFile(m_filePath).dispatch();
	}
	else if (text().size())
	{
		MessageProjectEdit().dispatch();
	}
}

void QtCodeFileTitleButton::copyFullPath()
{
	const std::string pathString = (QSysInfo::windowsVersion() != QSysInfo::WV_None) ? m_filePath.getBackslashedString() : m_filePath.str();
	QApplication::clipboard()->setText(pathString.c_str());
}

void QtCodeFileTitleButton::openContainingFolder()
{
	FilePath dir = m_filePath.parentDirectory();
	if (dir.exists())
	{
		QDesktopServices::openUrl(QUrl(("file:///" + dir.str()).c_str(), QUrl::TolerantMode));
	}
	else
	{
		LOG_ERROR("Unable to open directory: " + dir.str());
	}
}

