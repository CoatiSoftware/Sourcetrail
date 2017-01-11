#ifndef UTILITY_APP_H
#define UTILITY_APP_H

#include <QProcess>

#include "utility/ApplicationArchitectureType.h"

namespace utility
{
	std::string executeProcess(const char* cmd);

	ApplicationArchitectureType getApplicationArchitectureType();
}


#endif // UTILITY_APP_H

