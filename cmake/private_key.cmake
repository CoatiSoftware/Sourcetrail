# searching for private key file of the Sourcetrail version in setup/RSA_keys
# if found the file will be put into a const string into the PrivateKey.h file

string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GIT_VERSION_NUMBER}")
message(STATUS "Get Public Key for Sourcetrail version: ${VERSION_MAJOR}")

message(STATUS "${CMAKE_SOURCE_DIR}/setup/RSA_keys/private-sourcetrail.pem")

set(privateKeyFile "${CMAKE_SOURCE_DIR}/setup/RSA_keys/private-sourcetrail.pem")

if(EXISTS ${privateKeyFile})
    file(READ ${privateKeyFile} KEY)
    STRING(REGEX REPLACE "\"" "\\\\\"" KEY "${KEY}")
    STRING(REGEX REPLACE "\n" "\\\\n\"\n\t\"" KEY "${KEY}")
else()
	message(FATAL_ERROR "private keyfile for Sourcetrail not found")
endif()

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/PrivateKey.h.in
  ${CMAKE_BINARY_DIR}/src/license_generator/PrivateKey.h
)
