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
        std::string privateKey;
        std::string publicKey;
        std::string license;
        po::options_description desc("Coati Generator");

        desc.add_options()
            ("help,h", "Print this help message")
            ("key,k", "Generate the private and public key")
            ("generate,g", po::value<std::string>(&user), "Generate a License, USERNAME as value")
            ("check,c", "Validate a License")
            ("version,v", po::value<std::string>(&version), "Versionnumber of Coati")
            ("public-file", po::value<std::string>(&publicKey), "Custom public key file")
            ("private-file", po::value<std::string>(&privateKey), "Custom private key file")
            ("license-file", po::value<std::string>(&license), "Custom license")
             ;
        po::variables_map vm;

        po::store(po::parse_command_line(argc,argv,desc), vm);
        po::notify(vm);

        Generator keygen(version);

        if(vm.count("help"))
        {
            std::cout << desc << std::endl;
            return 1;
        }

        if (vm.count("key"))
        {
            keygen.generateKeys();
            keygen.writeKeysToFiles();
        }

        if (vm.count("public-file"))
        {
            keygen.setCustomPublicKeyFile(publicKey);
        }

        if (vm.count("private-file"))
        {
            keygen.setCustomPrivateKeyFile(privateKey);
        }

        if(vm.count("generate"))
        {
//            std::cout << "generate License" << std::endl;
            keygen.encodeLicense(user);

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
