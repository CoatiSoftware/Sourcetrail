#ifndef COMMANDLINE_SOURCEGROUP_OPTIONS_JAVA_H
#define COMMANDLINE_SOURCEGROUP_OPTIONS_JAVA_H

#include "utility/commandline/modes/CommandlineSourceGroupOptions.h"


class CommandlineSourceGroupOptionsJava : public CommandlineSourceGroupOptions
{
	virtual const std::string name() const;
	virtual void setup() override;
	virtual void parse(int argc, char** argv) override;

private:

};

#endif // COMMANDLINE_SOURCEGROUP_OPTIONS_JAVA_H
