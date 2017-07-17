#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <string>
#include "License.h"
#include "utility/file/FilePath.h"

class CommandLineParser
{
public:
    CommandLineParser(int argc, char** argv, const std::string& version);
    ~CommandLineParser();

	bool runWithoutGUI();
	bool exitApplication();
	bool startedWithLicense();
	bool hasError();

	std::string getError();
	License getLicense();

	const FilePath& getProjectFilePath() const;
	bool getFullProjectRefresh() const;

private:
	void processProjectfile(const std::string& file);
	void processLicense(const bool isLoaded);
	FilePath m_projectFile;

	bool m_force;
	bool m_quit;
	bool m_withLicense;
	bool m_withoutGUI;

	std::string m_errorString;
	License m_license;
};

#endif //COMMANDLINEPARSER_H
