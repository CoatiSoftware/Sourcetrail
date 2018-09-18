#include "QtCodeFileTitleButton.h"

#include "FileSystem.h"
#include "MessageActivateFile.h"
#include "MessageProjectEdit.h"
#include "ResourcePaths.h"
#include "utilityString.h"

#include "Application.h"
#include "Project.h"
#include "QtContextMenu.h"
#include "ColorScheme.h"

QtCodeFileTitleButton::QtCodeFileTitleButton(QWidget* parent)
	: QtSelfRefreshIconButton("", FilePath(), "code/file/title", parent)
	, m_isComplete(true)
	, m_isIndexed(true)
{
	setObjectName("title_button");
	minimumSizeHint(); // force font loading

	setFixedHeight(std::max(fontMetrics().height() * 1.2, 28.0));
	setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);

	setIconSize(QSize(16, 16));

	connect(this, &QtCodeFileTitleButton::clicked, this, &QtCodeFileTitleButton::clickedTitle);
}

QtCodeFileTitleButton::~QtCodeFileTitleButton()
{
}

const FilePath& QtCodeFileTitleButton::getFilePath() const
{
	return m_filePath;
}

void QtCodeFileTitleButton::setFilePath(const FilePath& filePath)
{
	setEnabled(true);

	m_filePath = filePath;

	updateIcon();
}

void QtCodeFileTitleButton::setModificationTime(const TimeStamp modificationTime)
{
	if (modificationTime.isValid())
	{
		m_modificationTime = modificationTime;
	}
}

void QtCodeFileTitleButton::setProject(const std::wstring& name)
{
	m_filePath = FilePath();

	setText(QString::fromStdWString(name));
	setToolTip("edit project");

	updateIcon();
}

bool QtCodeFileTitleButton::isComplete() const
{
	return m_isComplete;
}

void QtCodeFileTitleButton::setIsComplete(bool isComplete)
{
	if (m_isComplete == isComplete)
	{
		return;
	}

	m_isComplete = isComplete;
	setProperty("complete", isComplete);

	updateIcon();
}

bool QtCodeFileTitleButton::isIndexed() const
{
	return m_isIndexed;
}

void QtCodeFileTitleButton::setIsIndexed(bool isIndexed)
{
	if (m_isIndexed == isIndexed)
	{
		return;
	}

	m_isIndexed = isIndexed;
	setProperty("nonindexed", !isIndexed);

	updateHatching();
}

void QtCodeFileTitleButton::updateTexts()
{
	if (m_filePath.empty())
	{
		return;
	}

	std::wstring title = m_filePath.fileName();
	std::wstring toolTip = L"file: " + m_filePath.wstr();

	if (!m_isIndexed)
	{
		toolTip = L"non-indexed " + toolTip;
	}

	if (!m_isComplete)
	{
		toolTip = L"incomplete " + toolTip;
	}

	if ((!m_filePath.recheckExists()) ||
		(FileSystem::getLastWriteTime(m_filePath) > m_modificationTime))
	{
		title += L"*";
		toolTip = L"out-of-date " + toolTip;
	}

	setText(QString::fromStdWString(title));
	setToolTip(QString::fromStdWString(toolTip));
}

void QtCodeFileTitleButton::updateFromOther(const QtCodeFileTitleButton* other)
{
	if (!other->m_filePath.empty())
	{
		setFilePath(other->m_filePath);
	}
	else
	{
		setProject(other->text().toStdWString());
	}

	setModificationTime(other->m_modificationTime);
	setIsComplete(other->m_isComplete);
	setIsIndexed(other->m_isIndexed);
	updateTexts();
}

void QtCodeFileTitleButton::contextMenuEvent(QContextMenuEvent* event)
{
	FilePath path = m_filePath;
	if (path.empty())
	{
		Project* currentProject = Application::getInstance()->getCurrentProject().get();
		if (!currentProject)
		{
			return;
		}

		path = currentProject->getProjectSettingsFilePath();
	}

	QtContextMenu menu(event, this);
	menu.addSeparator();
	menu.addFileActions(path);
	menu.show();
}

void QtCodeFileTitleButton::refresh()
{
	QtSelfRefreshIconButton::refresh();

	updateHatching();
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

void QtCodeFileTitleButton::updateIcon()
{
	if (m_filePath.empty())
	{
		setIconPath(ResourcePaths::getGuiPath().concatenate(L"code_view/images/edit.png"));
	}
	else if (!m_isComplete)
	{
		setIconPath(ResourcePaths::getGuiPath().concatenate(L"graph_view/images/file_incomplete.png"));
	}
	else
	{
		setIconPath(ResourcePaths::getGuiPath().concatenate(L"code_view/images/file.png"));
	}
}

void QtCodeFileTitleButton::updateHatching()
{
	if (!m_isIndexed)
	{
		FilePath hatchingFilePath = ResourcePaths::getGuiPath().concatenate(L"code_view/images/pattern_" +
			utility::decodeFromUtf8(ColorScheme::getInstance()->getColor("code/file/title/hatching")) + L".png"
		);

		setStyleSheet(QString::fromStdWString(
			L"#title_button { background-image: url(" + hatchingFilePath.wstr() + L"); }"
		));
	}
	else
	{
		setStyleSheet("");
	}
}
