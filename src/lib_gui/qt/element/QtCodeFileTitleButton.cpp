#include "qt/element/QtCodeFileTitleButton.h"

#include "utility/file/FileSystem.h"
#include "utility/messaging/type/MessageActivateFile.h"
#include "utility/messaging/type/MessageProjectEdit.h"

#include "Application.h"
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
