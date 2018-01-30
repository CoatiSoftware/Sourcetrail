#include "utility/utilityGradle.h"

#include "data/parser/java/JavaEnvironment.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "utility/ResourcePaths.h"

namespace utility
{
	std::vector<std::wstring> getRequiredJarNames()
	{
		return {
			L"gradle-tooling-api-4.2.jar",
			L"java-indexer.jar",
			L"org.eclipse.core.commands-3.9.0.jar",
			L"org.eclipse.core.contenttype-3.6.0.jar",
			L"org.eclipse.core.expressions-3.6.0.jar",
			L"org.eclipse.core.filesystem-1.7.0.jar",
			L"org.eclipse.core.jobs-3.9.2.jar",
			L"org.eclipse.core.resources-3.12.0.jar",
			L"org.eclipse.core.runtime-3.13.0.jar",
			L"org.eclipse.equinox.app-1.3.400.jar",
			L"org.eclipse.equinox.common-3.9.0.jar",
			L"org.eclipse.equinox.preferences-3.7.0.jar",
			L"org.eclipse.equinox.registry-3.7.0.jar",
			L"org.eclipse.jdt.core-3.13.0.jar",
			L"org.eclipse.osgi-3.12.50.jar",
			L"org.eclipse.text-3.6.100.jar",
			L"slf4j-api-1.7.10.jar",
			L"slf4j-simple-1.7.10.jar"
		};
	}

	std::string prepareJavaEnvironment()
	{
		std::string errorString;

		if (!JavaEnvironmentFactory::getInstance())
		{
#ifdef _WIN32
			const std::string separator = ";";
#else
			const std::string separator = ":";
#endif

			std::string classPath = "";
			{
				const std::vector<std::wstring> jarNames = getRequiredJarNames();
				for (size_t i = 0; i < jarNames.size(); i++)
				{
					if (i != 0)
					{
						classPath += separator;
					}
					classPath += ResourcePaths::getJavaPath().concatenate(L"lib/" + jarNames[i]).str();
				}
			}

			JavaEnvironmentFactory::createInstance(
				classPath,
				errorString
			);
		}

		return errorString;
	}
}
