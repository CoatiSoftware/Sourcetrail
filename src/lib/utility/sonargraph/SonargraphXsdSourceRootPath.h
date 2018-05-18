#ifndef SONARGRAPH_XSD_SOURCE_ROOT_PATH_H
#define SONARGRAPH_XSD_SOURCE_ROOT_PATH_H

#include <memory>

#include "utility/sonargraph/SonargraphXsdRootPath.h"
#include "utility/types.h"

namespace Sonargraph
{
	class XsdSourceRootPath : public XsdRootPath
	{
	public:
		static std::string getXsdTypeName();
		static std::shared_ptr<XsdSourceRootPath> create(const TiXmlElement* element);

	protected:
		XsdSourceRootPath() = default;
		bool init(const TiXmlElement* element);
	};
}

#endif // SONARGRAPH_XSD_SOURCE_ROOT_PATH_H
