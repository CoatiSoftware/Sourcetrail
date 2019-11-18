#ifndef UTILITY_LIBRARY_H
#define UTILITY_LIBRARY_H

#include <functional>
#include <iostream>
#include <sstream>
#include <string>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <dlfcn.h>
#	define __stdcall
#endif

#include "FilePath.h"
#include "utilityString.h"

namespace utility
{
template <typename Ret, typename... Args>
std::function<Ret(Args...)> loadFunctionFromLibrary(
	const FilePath& libraryPath, const std::string& functionName, std::string& errorString)
{
#ifdef _WIN32
	const std::string libraryPathString = libraryPath.getBackslashedString();
	HINSTANCE handle = LoadLibrary(libraryPathString.c_str());
	if (handle == nullptr)
	{
		DWORD errorCode = GetLastError();
		std::string errorReasonString;

		LPSTR messageBuffer = nullptr;
		const size_t size = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			errorCode,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPSTR)&messageBuffer,
			0,
			nullptr);
		if (size > 0)
		{
			std::string message(messageBuffer, size);
			errorReasonString = "error \"" + utility::trim(message) + "\" (code " +
				std::to_string(errorCode) + ").";
		}
		else
		{
			errorReasonString = "error code " + std::to_string(errorCode) + ".";
		}

		LocalFree(messageBuffer);

		errorString = "Could not load library \"" + libraryPathString + "\" because of " +
			errorReasonString;
		return std::function<Ret(Args...)>();
	}

	FARPROC functionId = GetProcAddress(handle, functionName.c_str());

	if (!functionId)
	{
		errorString = "Could not locate the function \"" + functionName + "\" in library\"" +
			libraryPathString + "\"";
		return std::function<Ret(Args...)>();
	}
#else
	void* handle = dlopen(libraryPath.str().c_str(), RTLD_LAZY);
	if (handle == nullptr)
	{
		errorString = "Could not load library \"" + libraryPath.str() + "\"";
		return std::function<Ret(Args...)>();
	}

	void* functionId = (void*)dlsym(handle, functionName.c_str());

	const char* dlsym_error = dlerror();
	if (dlsym_error || !functionId)
	{
		std::stringstream ss;
		ss << "Cannot load symbol '" << functionName << "' from library '" << libraryPath.str()
		   << "': " << dlsym_error;
		errorString = ss.str();
		dlclose(handle);
		return std::function<Ret(Args...)>();
	}
#endif

	return std::function<Ret(Args...)>(reinterpret_cast<Ret(__stdcall*)(Args...)>(functionId));
}
}	 // namespace utility

#endif	  // UTILITY_LIBRARY_H
