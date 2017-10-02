#include "utility/utilityGradle.h"

#include "data/parser/java/JavaEnvironment.h"
#include "data/parser/java/JavaEnvironmentFactory.h"
#include "utility/ResourcePaths.h"

namespace utility
{
	std::vector<std::string> getRequiredJarNames()
	{
		std::vector<std::string> jarNames = {

			"gradle-tooling-api-4.2.jar",
			"java-indexer.jar",
			"org.eclipse.core.commands-3.8.1.jar",
			"org.eclipse.core.contenttype-3.5.100.jar",
			"org.eclipse.core.expressions-3.5.100.jar",
			"org.eclipse.core.filesystem-1.6.1.jar",
			"org.eclipse.core.jobs-3.8.0.jar",
			"org.eclipse.core.resources-3.11.1.jar",
			"org.eclipse.core.runtime-3.12.0.jar",
			"org.eclipse.equinox.app-1.3.400.jar",
			"org.eclipse.equinox.common-3.8.0.jar",
			"org.eclipse.equinox.preferences-3.6.1.jar",
			"org.eclipse.equinox.registry-3.6.100.jar",
			"org.eclipse.jdt.core-3.12.3.jar",
			"org.eclipse.osgi-3.11.3.jar",
			"org.eclipse.text-3.6.0.jar",
			"slf4j-api-1.7.10.jar",
			"slf4j-simple-1.7.10.jar"
		};
		return jarNames;
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
				const std::vector<std::string> jarNames = getRequiredJarNames();
				for (size_t i = 0; i < jarNames.size(); i++)
				{
					if (i != 0)
					{
						classPath += separator;
					}
					classPath += ResourcePaths::getJavaPath().str() + "lib/" + jarNames[i];
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
