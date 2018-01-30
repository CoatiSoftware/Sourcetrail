#ifndef UTILITY_JAVA_H
#define UTILITY_JAVA_H

#include <string>
#include <vector>

class FilePath;

namespace utility
{
	std::vector<std::wstring> getRequiredJarNames();
	std::string prepareJavaEnvironment();
}

#endif // UTILITY_JAVA_H
