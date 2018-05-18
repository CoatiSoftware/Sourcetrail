#ifndef SONARGRAPH_SOURCE_ROOT_PATH_H
#define SONARGRAPH_SOURCE_ROOT_PATH_H

#include <memory>
#include <string>
#include <vector>

#include "utility/file/FilePath.h"

class TiXmlElement;

namespace Sonargraph
{
	class SourceRootPath
	{
	public:
		static std::string getXsdTypeName();
		static std::shared_ptr<SourceRootPath> create(const TiXmlElement* element);

		virtual ~SourceRootPath() = default;

		std::wstring getName() const;

		FilePath getFilePath(const FilePath& baseDirectory) const;

	protected:
		SourceRootPath() = default;
		bool init(const TiXmlElement* element);

		std::wstring m_name;
	};
}

#endif // SONARGRAPH_SOURCE_ROOT_PATH_H
