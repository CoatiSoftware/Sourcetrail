#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

#include "boost/program_options.hpp"

#include "Generator.h"

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
        int seats = 0;

        po::options_description modes_description("Coati Generator Modes");
        modes_description.add_options()
            ("key,k", "Generate the private and public key")
            ("generate,g", po::value<std::string>(&user), "Generate a License, USERNAME as value")
            ("check,c", "Validate a License");

        po::options_description required("Required Options");
        required.add_options()
            ("version,v", po::value<std::string>(&version), "Versionnumber of Coati");

        po::options_description keygen_description("Options Keygeneration");
        keygen_description.add_options()
            ("seats,s", po::value<int>(&seats), "Generate a License, USERNAME as value")
            ("version,v", po::value<std::string>(&version), "Versionnumber of Coati")
            ("licenseType,t", po::value<std::string>(&type), "License Type of ")
            ("testLicense,e", po::value<int>(&days), "Generates a test license for <value> days");

        po::options_description hidden_description("Hidden Options");
        hidden_description.add_options()
            ("public-file", po::value<std::string>(&publicKeyFile), "Custom public key file")
            ("private-file", po::value<std::string>(&privateKeyFile), "Custom private key file")
            ("license-file", po::value<std::string>(&licenseFile), "Custom license")
            ("hidden", "Print this help message");

        po::options_description desc("Coati Generator");
        desc.add_options()
            ("help,h", "Print this help message");
        desc.add(modes_description).add(required).add(keygen_description);

        po::options_description allDescriptions("Coati Generator");
        allDescriptions.add_options();
        allDescriptions.add(modes_description).add(required).add(keygen_description).add(hidden_description);

        po::variables_map vm;

        po::store(po::parse_command_line(argc,argv,desc), vm);
        po::notify(vm);

        // display help if no argument or the help argument is given
        if (vm.count("help") || vm.size() == 0)
        {
            std::cout << desc << std::endl;
            return 1;
        }

        if (vm.count("hidden"))
        {
            std::cout << allDescriptions << std::endl;
            return 1;
        }

        // we need a version for all modes
        // if no version is given there is nothing to do
        if (!vm.count("version"))
        {
            std::cout << "No version specified" << std::endl;
            return false;
        }

        Generator keygen(version);

        // make sure there are no negative amount of seats
        if (vm.count("seats"))
        {
            if (seats < 0)
            {
                std::cout << "Invalid amount of seats. (Must be > 0)" << std::endl;
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

        if(vm.count("generate"))
        {
			if(vm.count("testLicense"))
			{
				keygen.encodeLicense(user,days);
			}
			else
			{
                keygen.encodeLicense(user,type,seats);
			}
        }

        if(vm.count("check"))
        {
            if(keygen.verifyLicense())
            {
                std::cout << "License valid" << std::endl;
            }
            else
            {
                std::cout << "License not valid" << std::endl;
            }
        }
    }
    catch(std::exception& e)
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
