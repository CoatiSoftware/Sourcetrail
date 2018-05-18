#ifndef SONARGRAPH_XSD_ABSTRACT_SYSTEM_EXTENSION_H
#define SONARGRAPH_XSD_ABSTRACT_SYSTEM_EXTENSION_H

#include <memory>
#include <string>

class TiXmlElement;

namespace Sonargraph
{
	class XsdAbstractSystemExtension
	{
	public:
		static std::string getXsdTypeName();
		static std::shared_ptr<XsdAbstractSystemExtension> create(const TiXmlElement* element);

		virtual ~XsdAbstractSystemExtension() = default;

	protected:
		XsdAbstractSystemExtension() = default;
		bool init(const TiXmlElement* element);

		std::string m_language;
	};
}

#endif // SONARGRAPH_XSD_ABSTRACT_SYSTEM_EXTENSION_H
