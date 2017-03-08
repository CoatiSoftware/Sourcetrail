#ifndef UTILITY_APP_H
#define UTILITY_APP_H

#include <string>

#include "utility/ApplicationArchitectureType.h"

namespace utility
{
	std::string executeProcess(const std::string& command, const std::string& workingDirectory = "");

	ApplicationArchitectureType getApplicationArchitectureType();
}

#endif // UTILITY_APP_H

