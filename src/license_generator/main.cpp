#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include "boost/program_options.hpp"

#include "Generator.h"
#include "PrivateKey.h"
#include "PublicKey.h"

namespace po = boost::program_options;

bool process_command_line(int argc, char** argv)
{
	try
	{
		std::string user;
		std::string version;
		std::string privateKeyFile;
		std::string publicKeyFile;
		std::string licenseFile = "";
		std::string type = "";
		int days = 0;
		int numberOfUsers = 0;

		po::options_description modes_description("Sourcetrail Generator Modes");
		modes_description.add_options()
			("key,k", "Generate the private and public key")
			("generate,g", po::value<std::string>(&user), "Generate a License, USERNAME as value")
			("check,c", "Validate a License");

		po::options_description keygen_description("Options Keygeneration");
		keygen_description.add_options()
			("version,v", po::value<std::string>(&version), "Versionnumber (in format 20xx.x) until Sourcetrail valid")
			("users,u", po::value<int>(&numberOfUsers), "Number of users")
			("seats,s", po::value<int>(&numberOfUsers), "Number of users (prev. seats)")
			("licenseType,t", po::value<std::string>(&type), "License Type of ")
			("testLicense,e", po::value<int>(&days), "Generates a test license for <value> days");

		po::options_description hidden_description("Hidden Options");
		hidden_description.add_options()
			("public-file", po::value<std::string>(&publicKeyFile), "Custom public key file")
			("private-file", po::value<std::string>(&privateKeyFile), "Custom private key file")
			("license-file", po::value<std::string>(&licenseFile), "Custom license")
			("hidden", "Print this help message");

		po::options_description desc("Sourcetrail Generator");
		desc.add_options()
			("help,h", "Print this help message");
		desc.add(modes_description).add(keygen_description);

		po::options_description allDescriptions("Sourcetrail Generator");
		allDescriptions.add_options();
		allDescriptions.add(modes_description).add(keygen_description).add(hidden_description);

		po::variables_map vm;

		po::store(po::parse_command_line(argc,argv,desc), vm);
		po::notify(vm);

		// display help if no argument or the help argument is given
		if (vm.count("help"))
		{
			std::cout << desc << std::endl;
			return 1;
		}

		// no mode chosen -> display help
		if ( vm.size() == 0 || !(vm.count("hidden") || vm.count("key") || vm.count("check") || vm.count("generate")))
		{
			std::cout << "*****************************\nNo mode chosen, display help: "
					  << "\n*****************************\n\n" << desc << std::endl;

			return 1;
		}


		if (vm.count("hidden"))
		{
			std::cout << allDescriptions << std::endl;
			return 1;
		}

		Generator keygen;

		// make sure there are no negative amount of users
		if (vm.count("users") || vm.count("seats"))
		{
			if (numberOfUsers < 0)
			{
				std::cout << "Invalid amount of users. (Must be > 0)" << std::endl;
				return false;
			}
		}

		if (vm.count("key"))
		{
			keygen.generateKeys();
			keygen.writeKeysToFiles();
		}

		if (vm.count("public-file"))
		{
			keygen.setCustomPublicKeyFile(publicKeyFile);
		}

		if (vm.count("private-file"))
		{
			keygen.setCustomPrivateKeyFile(privateKeyFile);
		}
		else
		{
			keygen.loadPrivateKeyFromString(PRIVATE_KEY);
		}

		if (vm.count("generate"))
		{
			if (vm.count("testLicense"))
			{
				keygen.encodeLicense(user, days);
				keygen.printLicenseAndWriteItToFile();
			}
			else
			{
				keygen.encodeLicense(user, type, numberOfUsers, version);
				keygen.printLicenseAndWriteItToFile();
			}
		}

		if (vm.count("check"))
		{
			if (keygen.verifyLicense())
			{
				std::cout << "License valid" << std::endl;
			}
			else
			{
				std::cout << "License not valid" << std::endl;
			}
		}
	}
	catch (std::exception& e)
	{
		std::cout << "Exception caught: " << e.what() << std::endl;
		return false;
	}

	return true;
}

int main(int argc, char *argv[]) {
	process_command_line(argc, argv);

	return 0;
}
