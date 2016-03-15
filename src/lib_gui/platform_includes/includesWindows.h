#ifndef INCLUDES_WINDOWS_H
#define INCLUDES_WINDOWS_H

#include <string>

#include "vld.h"

#include "utility/UserPaths.h"
#include "isTrial.h"

#include "platform_includes/deploy.h"

void setupPlatform(int argc, char *argv[])
{
}

void setupApp(int argc, char *argv[])
{
#ifdef DEPLOY
	std::string path = std::getenv("APPDATA");
	if (isTrial())
	{
		path += "/../local/Coati Software/Coati Trial/";
	}
	else
	{
		path += "/../local/Coati Software/Coati/";
	}
	UserPaths::setUserDataPath(path);
#endif
}

#endif // INCLUDES_WINDOWS_H
