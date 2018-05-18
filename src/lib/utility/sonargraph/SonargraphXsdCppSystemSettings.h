#ifndef SONARGRAPH_XSD_CPP_SYSTEM_SETTINGS_H
#define SONARGRAPH_XSD_CPP_SYSTEM_SETTINGS_H

#include <map>
#include <vector>

#include "utility/sonargraph/SonargraphXsdAbstractSystemExtension.h"
#include "utility/types.h"

namespace Sonargraph
{
	class XsdCppSystemSettings : public XsdAbstractSystemExtension
	{
	public:
		static std::string getXsdTypeName();
		static std::shared_ptr<XsdCppSystemSettings> create(const TiXmlElement* element);

		bool hasCompilerOptionsForId(Id id) const;
		std::vector<std::wstring> getCompilerOptionsForId(Id id) const;

	protected:
		XsdCppSystemSettings() = default;
		bool init(const TiXmlElement* element);

	private:
		std::map<Id, std::vector<std::wstring>> m_compilerOptionSets;
	};
}

#endif // SONARGRAPH_XSD_CPP_SYSTEM_SETTINGS_H
