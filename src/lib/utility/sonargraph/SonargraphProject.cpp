#include "utility/sonargraph/SonargraphProject.h"

#include "tinyxml/tinyxml.h"

#include "utility/sonargraph/SonargraphSoftwareSystem.h"
#include "utility/logging/logging.h"
#include "utility/text/TextAccess.h"
#include "utility/utilityString.h"
#include "utility/utilityXml.h"

namespace Sonargraph
{
	std::shared_ptr<Project> Project::load(const FilePath& projectFilePath, LanguageType targetLanguage)
	{
		return load(TextAccess::createFromFile(projectFilePath), targetLanguage);
	}

	std::shared_ptr<Project> Project::load(std::shared_ptr<TextAccess> xmlAccess, LanguageType targetLanguage)
	{
		if (!xmlAccess)
		{
			return std::shared_ptr<Project>();
		}

		std::shared_ptr<Project> project = std::shared_ptr<Project>(new Project());

		TiXmlDocument doc;
		doc.Parse(xmlAccess->getText().c_str(), 0, TIXML_ENCODING_UTF8);
		if (doc.Error())
		{
			LOG_ERROR(
				"Unable to parse Sonargraph project because of an error in row " + std::to_string(doc.ErrorRow()) + ", col " +
				std::to_string(doc.ErrorCol()) + ": " + std::string(doc.ErrorDesc())
			);
			return std::shared_ptr<Project>();
		}

		TiXmlHandle docHandle(&doc);
		TiXmlElement* softwareSystemElement = docHandle.FirstChildElement("ns2:softwareSystem").ToElement();
		if (softwareSystemElement == nullptr)
		{
			LOG_ERROR("Unable to find \"ns2:softwareSystem\" in Sonargraph project.");
			return std::shared_ptr<Project>();
		}

		std::shared_ptr<SoftwareSystem> softwareSystem = SoftwareSystem::create(
			softwareSystemElement, xmlAccess->getFilePath().getParentDirectory().getParentDirectory(), targetLanguage
		);
		if (!softwareSystem)
		{
			return std::shared_ptr<Project>();
		}

		project->m_softwareSystem = softwareSystem;
		return project;
	}

	int Project::getLoadedModuleCount() const
	{
		return m_softwareSystem->getModules().size();
	}

	std::set<FilePath> Project::getAllSourcePaths() const
	{
		return m_softwareSystem->getAllSourcePaths();
	}

	std::set<FilePath> Project::getAllSourceFilePathsCanonical() const
	{
		return m_softwareSystem->getAllSourceFilePathsCanonical();
	}

	std::set<FilePath> Project::getAllCxxHeaderSearchPathsCanonical() const
	{
		return m_softwareSystem->getAllCxxHeaderSearchPathsCanonical();
	}

	std::set<FilePath> Project::filterToContainedFilePaths(const std::set<FilePath>& filePaths) const
	{
		return m_softwareSystem->filterToContainedFilePaths(filePaths);
	}

	std::vector<std::shared_ptr<IndexerCommand>> Project::getIndexerCommands(
		std::shared_ptr<const SourceGroupSettings> sourceGroupSettings, 
		std::shared_ptr<const ApplicationSettings> appSettings) const
	{
		return m_softwareSystem->getIndexerCommands(sourceGroupSettings, appSettings);
	}
}
