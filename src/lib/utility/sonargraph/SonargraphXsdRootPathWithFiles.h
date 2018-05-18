#ifndef SONARGRAPH_XSD_ROOT_PATH_WITH_FILES_H
#define SONARGRAPH_XSD_ROOT_PATH_WITH_FILES_H

#include <memory>

#include "utility/sonargraph/SonargraphXsdRootPath.h"
#include "utility/types.h"

namespace Sonargraph
{
	class XsdRootPathWithFiles : public XsdRootPath
	{
	public:
		struct SourceFile
		{
			SourceFile(std::wstring fileName, Id compilerOptionSetId);

			FilePath getFilePath(const FilePath& baseDirectory) const;

			std::wstring fileName;
			Id compilerOptionSetId;
		};

		static std::string getXsdTypeName();
		static std::shared_ptr<XsdRootPathWithFiles> create(const TiXmlElement* element);

		std::vector<SourceFile> getSourceFiles() const;
		std::vector<std::wstring> getExcludedDirectories() const;

	protected:
		XsdRootPathWithFiles() = default;
		bool init(const TiXmlElement* element);

		std::vector<SourceFile> m_sourceFiles;
		std::vector<std::wstring> m_excludedDirectories;
	};
}

#endif // SONARGRAPH_XSD_ROOT_PATH_WITH_FILES_H
