#ifndef QTCOMMANDLINEPARSER_H
#define QTCOMMANDLINEPARSER_H

#include <QCommandLineParser>
#include "Application.h"

class QtCommandLineParser : public QCommandLineParser
{
public:
    QtCommandLineParser();
    ~QtCommandLineParser();
    void setup();
    void parseCommandline();

private:

};

#endif //QTCOMMANDLINEPARSER_H
