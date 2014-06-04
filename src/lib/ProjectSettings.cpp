#include "ProjectSettings.h"

#include "utility/FileSystem.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"

std::shared_ptr<ProjectSettings> ProjectSettings::s_instance;

std::shared_ptr<ProjectSettings> ProjectSettings::getInstance()
{
	if (!s_instance)
	{
		s_instance = std::shared_ptr<ProjectSettings>(new ProjectSettings());
	}

	return s_instance;
}

ProjectSettings::ProjectSettings()
{
}

ProjectSettings::~ProjectSettings()
{
}

bool ProjectSettings::load(const std::string& projectSettingsFilePath)
{
	m_config.reset();
	if (FileSystem::exists(projectSettingsFilePath))
	{
		m_config = ConfigManager::createAndLoad(TextAccess::createFromFile(projectSettingsFilePath));
		return true;
	}
	else
	{
		LOG_WARNING("File for Projectsettings not found");
		return false;
	}
}

void ProjectSettings::save(const std::string& projectSettingsFilePath)
{
	if (m_config)
	{
		m_config->save();
	}
}

std::string ProjectSettings::getSourcePath() const
{
	if (m_config)
	{
		std::string sourcePath;
		if (m_config->getValue("SourcePath", sourcePath))
		{
			return sourcePath;
		}
	}
	return "";
}

void ProjectSettings::setSourcePath(const std::string& sourcePath)
{
	if (m_config)
	{
		m_config->setValue("SourcePath", sourcePath);
	}
}
