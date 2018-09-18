#include "SonargraphXsdRootPathWithFiles.h"

#include "tinyxml.h"

#include "utilitySonargraph.h"
#include "logging.h"
#include "utilityString.h"
#include "utilityXml.h"

namespace Sonargraph
{
	XsdRootPathWithFiles::SourceFile::SourceFile(std::wstring fileName, Id compilerOptionSetId)
		: fileName(fileName)
		, compilerOptionSetId(compilerOptionSetId)
	{
	}

	FilePath XsdRootPathWithFiles::SourceFile::getFilePath(const FilePath& baseDirectory) const
	{
		FilePath filePath(fileName);
		if (filePath.isAbsolute())
		{
			return filePath;
		}
		return baseDirectory.getConcatenated(filePath).makeCanonical();
	}


	std::string XsdRootPathWithFiles::getXsdTypeName()
	{
		return "xsdRootPathWithFiles";
	}

	std::shared_ptr<XsdRootPathWithFiles> XsdRootPathWithFiles::create(const TiXmlElement* element)
	{
		if (!utility::xmlElementHasAttribute(element, "xsi:type") || utility::sonargraphXmlElementIsType(element, getXsdTypeName()))
		{
			std::shared_ptr<XsdRootPathWithFiles> rootPath = std::shared_ptr<XsdRootPathWithFiles>(new XsdRootPathWithFiles());
			if (rootPath->init(element))
			{
				return rootPath;
			}
		}
		return std::shared_ptr<XsdRootPathWithFiles>();
	}

	std::vector<XsdRootPathWithFiles::SourceFile> XsdRootPathWithFiles::getSourceFiles() const
	{
		return m_sourceFiles;
	}

	std::vector<std::wstring> XsdRootPathWithFiles::getExcludedDirectories() const
	{
		return m_excludedDirectories;
	}

	bool XsdRootPathWithFiles::init(const TiXmlElement* element)
	{
		XsdRootPath::init(element);

		if (element != nullptr)
		{
			for (const TiXmlElement* sourceFileElement : utility::getXmlChildElementsWithName(element, "sourceFile"))
			{
				std::wstring fileName;
				{
					const char* value = sourceFileElement->Attribute("fileName");
					if (value != nullptr)
					{
						fileName = utility::decodeFromUtf8(value);
					}
					else
					{
						LOG_ERROR("Unable to parse \"fileName\" attribute of Sonargraph " + getXsdTypeName() + ".");
						return false;
					}
				}

				Id compilerOptionSetId;
				{
					int value;
					if (sourceFileElement->QueryIntAttribute("compilerOptionSetId", &value) == TIXML_SUCCESS && value >= 0)
					{
						compilerOptionSetId = Id(value);
					}
					else
					{
						LOG_ERROR("Unable to parse \"compilerOptionSetId\" attribute of Sonargraph " + getXsdTypeName() + ".");
						return false;
					}
				}

				m_sourceFiles.push_back(SourceFile(fileName, compilerOptionSetId));
			}
			for (const TiXmlElement* excludedDirectoryElement : utility::getXmlChildElementsWithName(element, "excludedDirectory"))
			{
				const char* value = excludedDirectoryElement->Attribute("dir");
				if (value != nullptr)
				{
					m_excludedDirectories.push_back(utility::decodeFromUtf8(value));
				}
				else
				{
					LOG_ERROR("Unable to parse \"dir\" attribute of Sonargraph " + getXsdTypeName() + ".");
					return false;
				}
			}
			return true;
		}
		return false;
	}
}
