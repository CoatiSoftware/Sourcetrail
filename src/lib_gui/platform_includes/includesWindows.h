#ifndef INCLUDES_WINDOWS_H
#define INCLUDES_WINDOWS_H

#include <string>

#include "vld.h"

#include "utility/UserPaths.h"

// #define DEPLOY

void setup(int argc, char *argv[])
{
#ifdef DEPLOY
	UserPaths::setUserDataPath(std::getenv("APPDATA")) + "/../local/Coati Software/Coati/");
#endif
}

#endif // INCLUDES_WINDOWS_H
