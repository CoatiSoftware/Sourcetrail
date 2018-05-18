#ifndef SONARGRAPH_XSD_ROOT_PATH_H
#define SONARGRAPH_XSD_ROOT_PATH_H

#include <memory>
#include <string>
#include <vector>

#include "utility/file/FilePath.h"

class IndexerCommand;
class SonargraphAbstractModule;
class TiXmlElement;

namespace Sonargraph
{
	class XsdRootPath
	{
	public:
		static std::string getXsdTypeName();
		static std::shared_ptr<XsdRootPath> create(const TiXmlElement* element);

		virtual ~XsdRootPath() = default;

		std::wstring getName() const;

		FilePath getFilePath(const FilePath& baseDirectory) const;

	protected:
		XsdRootPath() = default;
		bool init(const TiXmlElement* element);

		std::wstring m_name;
	};
}

#endif // SONARGRAPH_XSD_ROOT_PATH_H
