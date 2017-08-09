#include "CommandlineSourceGroupOptionsJava.h"

#include <iostream>

namespace po = boost::program_options;

const std::string CommandlineSourceGroupOptionsJava::name() const
{
	return "java";
}

void CommandlineSourceGroupOptionsJava::setup()
{
	m_options.add_options()
		("help,h", "Print this help message")
		("mavenpath,m", po::value<std::string>(), "path to the maven executable")
		("jvmpath,j", po::value<std::string>(), "path the jvm root")
		("excludePaths,e", po::value<std::vector<std::string>>(), "files or directories that will be excludes indexing")
		("source-extensions", po::value<std::string>(), "extension of Sourcefiles")
		("java-standard", po::value<int>(), "Java Standard")
		("classpath", po::value<std::vector<std::string>>(), "Java Class Paths")
		;
}

void CommandlineSourceGroupOptionsJava::parse(int argc, char** argv)
{
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(m_options).run(), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << m_options << std::endl;
	}

	if (vm.count("config-file"))
	{

	}


}
