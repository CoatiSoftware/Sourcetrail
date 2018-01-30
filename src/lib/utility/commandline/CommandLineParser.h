#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "boost/program_options.hpp"

#include "License.h"
#include "project/RefreshInfo.h"
#include "utility/file/FilePath.h"

namespace po = boost::program_options;

namespace commandline {

class Command;

class CommandLineParser
{
public:

	CommandLineParser(const std::string& version);
    ~CommandLineParser();

	void setup();

	void preparse(int argc, char** argv);

	/// args will be moved so dont use them after calling this function
	void preparse(std::vector<std::string>& args);
	void parse();

	bool runWithoutGUI();
	bool exitApplication();
	bool startedWithLicense();
	bool hasError();

	void fullRefresh();
	void incompleteRefresh();

	std::wstring getError();
	License getLicense();
	License* getLicensePtr();

	const FilePath& getProjectFilePath() const;
	void setProjectFile(const FilePath& filepath);
	RefreshMode getRefreshMode() const;

private:
	void addCommand(std::unique_ptr<Command> command);
	void processProjectfile();
	void processLicense(const bool isLoaded);
	void printHelp() const;
	FilePath m_projectFile;

	std::vector<std::shared_ptr<Command>> m_commands;

	const std::string m_version;
	RefreshMode m_refreshMode = REFRESH_UPDATED_FILES;
	bool m_quit{false};
	bool m_withoutGUI{false};

	boost::program_options::options_description m_options;
	boost::program_options::positional_options_description m_positional;

	std::wstring m_errorString;
	License m_license;

	std::vector<std::string> m_args;
};

} // namespace cmd

#endif //COMMANDLINEPARSER_H
