#include "qt/element/QtCodeFileTitleButton.h"

#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageActivateFile.h"
#include "utility/messaging/type/MessageProjectEdit.h"

#include "project/Project.h"
#include "qt/utility/QtContextMenu.h"
#include "qt/utility/utilityQt.h"
#include "settings/ColorScheme.h"
#include "utility/ResourcePaths.h"
#include "Application.h"

QtCodeFileTitleButton::QtCodeFileTitleButton(QWidget* parent)
	: QPushButton(parent)
	, m_isComplete(true)
{
	setObjectName("title_label");
	minimumSizeHint(); // force font loading
	setAttribute(Qt::WA_LayoutUsesWidgetRect); // fixes layouting on Mac

	setFixedHeight(std::max(fontMetrics().height() * 1.2, 28.0));
	setSizePolicy(sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);

	connect(this, &QtCodeFileTitleButton::clicked, this, &QtCodeFileTitleButton::clickedTitle);
}

QtCodeFileTitleButton::~QtCodeFileTitleButton()
{
}

void QtCodeFileTitleButton::setFilePath(const FilePath& filePath)
{
	m_filePath = filePath;
	setText("");

	setText(filePath.fileName().c_str());
	setToolTip(filePath.str().c_str());

	std::string text = ResourcePaths::getGuiPath().str() + "code_view/images/file.png";

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
		updateTexts();
	}
}

void QtCodeFileTitleButton::setIsComplete(bool isComplete)
{
	if (m_isComplete == isComplete)
	{
		return;
	}

	m_isComplete = isComplete;
	setProperty("complete", isComplete);

	if (!isComplete)
	{
		FilePath hatchingFilePath(ResourcePaths::getGuiPath().str() + "code_view/images/pattern_" +
			ColorScheme::getInstance()->getColor("code/file/title/hatching") + ".png"
		);

		setStyleSheet((
			"#title_label, #file_title { background-image: url(" + hatchingFilePath.str() + "); }"
		).c_str());
	}
	else
	{
		setStyleSheet("");
	}

	updateTexts();
}

void QtCodeFileTitleButton::setProject(const std::string& name)
{
	setText(name.c_str());
	m_filePath = FilePath();

	if (Application::getInstance()->isInTrial())
	{
		setEnabled(false);
	}
	else
	{
		std::string text = ResourcePaths::getGuiPath().str() + "code_view/images/edit.png";
		setToolTip("edit project");

		setIcon(utility::colorizePixmap(
			QPixmap(text.c_str()),
			ColorScheme::getInstance()->getColor("code/file/title/icon").c_str()
		));
	}
}

void QtCodeFileTitleButton::updateTexts()
{
	if (Application::getInstance()->isInTrial() || m_filePath.empty())
	{
		return;
	}

	std::string title = m_filePath.fileName();
	std::string toolTip = "file: " + m_filePath.str();

	if ((!m_filePath.recheckExists()) ||
		(FileSystem::getLastWriteTime(m_filePath) > m_modificationTime))
	{
		title += "*";
		toolTip = "out of date " + toolTip;
	}

	if (!m_isComplete)
	{
		toolTip = "incomplete " + toolTip;
	}

	setText(title.c_str());
	setToolTip(toolTip.c_str());
}

void QtCodeFileTitleButton::contextMenuEvent(QContextMenuEvent* event)
{
	QtContextMenu menu(event, this);
	menu.addSeparator();

	if (!m_filePath.empty())
	{
		menu.addFileActions(m_filePath);
	}
	else if (text().size())
	{
		Project* currentProject = Application::getInstance()->getCurrentProject().get();
		if (!currentProject)
		{
			return;
		}

		menu.addFileActions(currentProject->getProjectSettingsFilePath());
	}

	menu.show();
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
