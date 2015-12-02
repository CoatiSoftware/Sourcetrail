#ifndef INCLUDES_WINDOWS_H
#define INCLUDES_WINDOWS_H

#include <string>

#include "vld.h"

//#define DEPLOY

#ifdef DEPLOY
	static std::string appSettingsPath = std::string(std::getenv("APPDATA")) + "/../local/Coati Software/Coati/ApplicationSettings.xml";
	static std::string windowSettingsPath = std::string(std::getenv("APPDATA")) + "/../local/Coati Software/Coati/window_settings.ini";
	static std::string logPath = std::string(std::getenv("APPDATA")) + "/../local/Coati Software/Coati/log/";
#else
	static std::string appSettingsPath = "data/ApplicationSettings.xml";
	static std::string windowSettingsPath = "data/window_settings.ini";
	static std::string logPath = "data/log/";
#endif

void setup(int argc, char *argv[])
{
	
}

#endif // INCLUDES_WINDOWS_H
