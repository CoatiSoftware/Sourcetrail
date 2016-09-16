#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <string>
#include "Application.h"
#include "License.h"

class CommandLineParser
{
public:
    CommandLineParser(int argc, char** argv, const std::string& version);
    ~CommandLineParser();

	bool runWithoutGUI();
	bool exitApplication();
	void projectLoad();
	bool startedWithLicense();
	License getLicense();
private:
	void processProjectfile(const std::string& file);
	void processLicense(const bool isLoaded);
	std::string m_projectFile;

	bool m_force;
	bool m_quit;
	bool m_withLicense;
	bool m_withoutGUI;

	License m_license;
};

#endif //COMMANDLINEPARSER_H
