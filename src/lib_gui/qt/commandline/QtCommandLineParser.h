#ifndef QTCOMMANDLINEPARSER_H
#define QTCOMMANDLINEPARSER_H

#include <string>
#include <QCommandLineParser>
#include "Application.h"

class QtCommandLineParser : public QCommandLineParser
{
public:
    QtCommandLineParser();
    ~QtCommandLineParser();
    void setup();
    void evaluateCommandline();
    void projectLoad();
    bool noGUI();

private:
    std::string m_projectFileString;
    bool m_noGUI;

    void processProjectfile(const std::string& file);
};

#endif //QTCOMMANDLINEPARSER_H

