#ifndef INCLUDES_DEFAULT_H
#define INCLUDES_DEFAULT_H

#include "utility/UserPaths.h"

void setup(int argc, char *argv[])
{
    std::string userdir(std::getenv("HOME"));
    userdir.append("/.config/coati/");
    UserPaths::setUserDataPath(userdir);
}

#endif // INCLUDES_DEFAULT_H