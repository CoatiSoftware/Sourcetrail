#ifndef UTILITY_LIBRARY_H
#define UTILITY_LIBRARY_H

#include <iostream>
#include <functional>
#include <sstream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#define __stdcall
#endif

#include "utility/file/FilePath.h"

namespace utility
{
	template <typename Ret, typename... Args>
	std::function<Ret(Args...)> loadFunctionFromLibrary(
			const FilePath& libraryPath,
			const std::string& functionName,
			std::string& errorString)
	{
#ifdef _WIN32
		const std::string libraryPathString = libraryPath.getBackslashedString();
		HINSTANCE handle = LoadLibrary(libraryPathString.c_str());
		if (handle == NULL)
		{
			errorString = "Could not load library \"" + libraryPathString + "\"";
			return std::function<Ret(Args...)>();
		}

		FARPROC functionId = GetProcAddress(handle, functionName.c_str());

		if (!functionId)
		{
			errorString = "Could not locate the function \"" + functionName
				+ "\" in library\"" + libraryPathString + "\"";
			return std::function<Ret(Args...)>();
		}
#else
		void* handle = dlopen(libraryPath.str().c_str(), RTLD_LAZY);
		if (handle == nullptr)
		{
			errorString = "Could not load library \"" + libraryPath.str() + "\"";
			return std::function<Ret(Args...)>();
		}

		void* functionId = (void*) dlsym(handle, functionName.c_str());

		const char *dlsym_error = dlerror();
		if (dlsym_error || !functionId)
		{
			std::stringstream ss;
			ss << "Cannot load symbol '" << functionName << "' from library '" << libraryPath.str() << "': " << dlsym_error;
			errorString = ss.str();
			dlclose(handle);
			return std::function<Ret(Args...)>();
		}
#endif

		return std::function<Ret(Args...)>(reinterpret_cast<Ret (__stdcall *)(Args...)>(functionId));
	}
}

#endif // UTILITY_LIBRARY_H
