#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include <boost/program_options.hpp>

#include "LicenseChecker.h"
#include "LicenseGenerator.h"
#include "PrivateKey.h"
#include "PublicKey.h"

namespace po = boost::program_options;

bool process_command_line(int argc, char** argv)
{
	try
	{
		std::string user;
		std::string version;
		std::string privateKeyFile = "private-sourcetrail.pem";
		std::string publicKeyFile = "public-sourcetrail.pem";
		std::string licenseFile = "license.txt";
		std::string type = "";
		int quarters = 4;
		int days = 0;
		int numberOfUsers = 0;

		po::options_description modes_description("Sourcetrail Generator Modes");
		modes_description.add_options()
			("key,k", "Generate the private and public key")
			("generate,g", po::value<std::string>(&user), "Generate a License, USERNAME as value")
			("check,c", "Validate a License");

		po::options_description keygen_description("Options License Generation");
		keygen_description.add_options()
			("version,v", po::value<std::string>(&version), "Versionnumber (in format 20xx.x) until Sourcetrail valid")
			("quarters,q", po::value<int>(&quarters), "Number of quarters Sourcetrail is valid from now")
			("users,u", po::value<int>(&numberOfUsers), "Number of users")
			("licenseType,t", po::value<std::string>(&type), "License Type of ")
			("expiration,e", po::value<int>(&days), "Valid for <value> days")
			("lifelong,l", "Valid perpetually");

		po::options_description advanced_description("Advanced Options");
		advanced_description.add_options()
			("public-file", po::value<std::string>(&publicKeyFile), "Custom public key file")
			("private-file", po::value<std::string>(&privateKeyFile), "Custom private key file")
			("license-file", po::value<std::string>(&licenseFile), "Custom license (default: license.txt)");


		po::options_description desc("Sourcetrail Generator");
		desc.add_options()
			("help,h", "Print this help message");
		desc.add(modes_description).add(keygen_description).add(advanced_description);

		po::variables_map vm;
		po::store(po::parse_command_line(argc,argv,desc), vm);
		po::notify(vm);

		// display help if no argument or the help argument is given
		if (vm.count("help"))
		{
			std::cout << desc << std::endl;
			return true;
		}

		// no mode chosen -> display help
		if ( vm.size() == 0 || !(vm.count("hidden") || vm.count("key") || vm.count("check") || vm.count("generate")))
		{
			std::cout << "*****************************\nNo mode chosen, display help: "
					  << "\n*****************************\n\n" << desc << std::endl;

			return false;
		}

		LicenseGenerator keygen;

		if (vm.count("key"))
		{
			keygen.generatePrivateKey();
			keygen.writeKeysToFiles(publicKeyFile, privateKeyFile);
			return true;
		}

		if (vm.count("private-file"))
		{
			keygen.loadPrivateKeyFromFile(privateKeyFile);
		}
		else
		{
			keygen.loadPrivateKeyFromString(PRIVATE_KEY);
		}

		if (vm.count("generate"))
		{
			// make sure there is no negative amount of users
			if (vm.count("users"))
			{
				if (numberOfUsers < 0)
				{
					std::cout << "Invalid amount of users. (Must be > 0)" << std::endl;
					return false;
				}
			}

			std::unique_ptr<License> license;

			if (vm.count("lifelong"))
			{
				license = keygen.createLicenseLifelong(user, type, numberOfUsers);
			}
			else if (vm.count("expiration"))
			{
				license = keygen.createLicenseByDays(user, type, numberOfUsers, days);
			}
			else if (!version.empty())
			{
				license = keygen.createLicenseByVersion(user, type, numberOfUsers, version);
			}
			else
			{
				license = keygen.createLicenseByQuarters(user, type, numberOfUsers, quarters);
			}

			if (license)
			{
				license->print();
				license->writeToFile(licenseFile);
			}
		}

		if (vm.count("check"))
		{
			if (vm.count("public-file"))
			{
				LicenseChecker::loadPublicKeyFromFile(publicKeyFile);
			}
			else
			{
				LicenseChecker::loadPublicKey();
			}

			License license;
			LicenseChecker::LicenseState state = LicenseChecker::LicenseState::EMPTY;
			if (license.loadFromFile(licenseFile))
			{
				state = LicenseChecker::checkLicense(license);
			}
			else
			{
				state = LicenseChecker::LicenseState::MALFORMED;
			}

			if (state == LicenseChecker::LicenseState::VALID)
			{
				std::cout << "License valid" << std::endl;
			}
			else
			{
				std::cout << LicenseChecker::getLicenseErrorForState(state) << std::endl;
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

int main(int argc, char *argv[])
{
	process_command_line(argc, argv);

	return 0;
}
